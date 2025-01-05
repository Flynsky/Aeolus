/**
 * This script a debug console running on the STM32
 */
#include "console.h"
#include "packet.c"
#include "i2c_scan.c"

// console
const char END_OF_TRAMISSION_MARKER = 4; // EOT character 0x04
const uint8_t PACKAGE_SPECIFIER = 3;
uint8_t VCP_Buffer_rx[sizeof(struct message)] = {0};

static void message_handler(struct message *msg);
static void byte_to_msg(uint8_t *byte_stream);

// debugging tools
void debugf(const char *__restrict format, ...);

void print_bytes(void *ptr, unsigned int size);
void packet_send_test(void);
extern I2C_HandleTypeDef hi2c1;
void i2c_scan(I2C_HandleTypeDef *hi2c);
static void JumpToBootloader(void);

//-----------------Handle Message-----------------------------
/**
 * Takes a valid message and runs it`s command
 */
void message_handler(struct message *msg)
{
    debugf("Recieved: [%c] [%.2f]\n", msg->command0, msg->param0);
    // print_bytes(msg, sizeof(struct message));

    switch ((int)msg->command0)
    {
    case CTRL_HELP:
        debugf("Help:\n");
        debugf("Use commands to control this device.\n");
        debugf("[command] [parameter]\n");
        debugf("['?'] get_help \n");
        debugf("['h'] Enter Boot mode\n");
        debugf("['s'] Scan for I2C devices\n");
        debugf("['p'] [Amount] send test packet(s)\n");

        // debugf("['v'] [] reads out all voltages/currents\n");
        break;

    case CTRL_SEND_TEST_PACKET:
        if (msg->param0 <= 1)
        {
            debugf("send test packet\n");
            packet_send_test();
        }
        else
        {
            debugf("send %.2f test packet\n", msg->param0);
            for (int i = 0; i < msg->param0; i++)
            {
                packet_send_test();
            }
        }
        break;

    case CTRL_RESTART:
        debugf("Restart uC Highside\n");
        NVIC_SystemReset();
        break;

    case CTRL_I2C_SCAN:
        i2c_scan(&hi2c1);
        break;

    case CTRL_BOOT:
        debugf("Restarting in DFU Bootloader mode...\n");
        JumpToBootloader();
        break;

    default:
        break;
    }

    free(msg);
}

//-----------------Recieve Messages-------------------------------------
#if CONSOLE_MODE == CONSOLE_MODE_VCOM

/**
 * This is necessary to be able to recieve packages via a virtual com port
 */
void console_init()
{
    //"arms" CDC buffer
    USBD_CDC_SetRxBuffer(&hUsbDeviceFS, VCP_Buffer_rx);
    USBD_CDC_ReceivePacket(&hUsbDeviceFS);
    VCP_Buffer_rx[0] = 0;
}

/**
 * checks weather a new message via com got recieved.
 */
void console_check_for_messages()
{
    if (VCP_Buffer_rx[0] != 0)
    {
        byte_to_msg(VCP_Buffer_rx);
        VCP_Buffer_rx[0] = 0;
    }
}

// #include "usbd_cdc_if.c"

/**
 * If a message from a virtual com port got recieved,
 * this interupt triggers to read this message in the VCP_Buffer_rx and
 * gives the message bytestream to the next method byte_to_msg
 */
int8_t CDC_Receive_FS(uint8_t *Buf, uint32_t *Len)
{
    (void)Buf; // mak as unused
    (void)Len;

    // debugf("uC message [%i] received\n", (unsigned int)(*Len));

    // Prepare to receive the next packet
    USBD_CDC_SetRxBuffer(&hUsbDeviceFS, VCP_Buffer_rx);
    USBD_CDC_ReceivePacket(&hUsbDeviceFS);
    return USBD_OK;
}

#elif CONSOLE_MODE == CONSOLE_MODE_UART

extern UART_HandleTypeDef *huart_com;
uint8_t rxBuffer[sizeof(struct message)] = {0}; // buffer of incommming rx data aka messages

/**
 * This is necessary to be able to recieve packages via a virtual com port
 */
void console_init()
{
    // debugf("initialising console\n");
    HAL_StatusTypeDef ret = HAL_UART_Receive_DMA(huart_com, rxBuffer, sizeof(rxBuffer)); // activates DMA and its interupt for and in console.c
    switch (ret)
    {
    case HAL_OK:
        // debugf("Console_UART: HAL_OK\n");
        break;

    case HAL_ERROR:
        debugf("Console_UART: HAL_ERROR\n");
        break;

    case HAL_BUSY:
        debugf("Console_UART: HAL_BUSY\n");
        break;

    case HAL_TIMEOUT:
        debugf("Console_UART: HAL_TIMEOUT\n");
        break;

    default:
        debugf("Console_UART: UNDEF\n");
        break;
    }
}

/**
 * If the DMA from USART gets full aka a message got recieved,
 * this interupt triggers to read out the rxBuffer and
 * gives the bytestream to the next method
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    // debugf("recieved USART\n");
    byte_to_msg(rxBuffer);
    HAL_StatusTypeDef ret = HAL_UART_Receive_DMA(huart, rxBuffer, sizeof(rxBuffer));
    switch (ret)
    {
    case HAL_OK:
        break;

    case HAL_ERROR:
        debugf("Console_UART: HAL_ERROR\n");
        break;

    case HAL_BUSY:
        debugf("Console_UART: HAL_BUSY\n");
        break;

    case HAL_TIMEOUT:
        debugf("Console_UART: HAL_TIMEOUT\n");
        break;

    default:
        debugf("Console_UART: UNDEF\n");
        break;
    }
}

// #else
// void console_init()
// {
//     // no console
// }

#endif

/**
 * Converts a bytestream to a message
 */
void byte_to_msg(uint8_t *byte_stream)
{
    // debugf("bytetomessage\n");
    if (byte_stream[0] == NODATA_AVALIABLE)
    {
        debugf("no USART data\n");
        return;
    }

    // convert to struct message
    struct message *msg = (struct message *)malloc(sizeof(struct message));
    if (!msg)
    {
        debugf("SerialHandler Message malloc Error\n");
        byte_stream[0] = NODATA_AVALIABLE;
        return;
    }

    memcpy(msg, byte_stream, sizeof(struct message));
    // print_bytes(byte_stream, sizeof(struct message));
    // print_bytes(msg, sizeof(struct message));

    // clears rxBuffer
    byte_stream[0] = NODATA_AVALIABLE;

    // if struct message valid
    if ((msg->command0 != msg->command1) || (msg->command0 != msg->command2))
    {
        debugf("Message command corrupted\n");
        return;
    }

    if ((msg->param0 != msg->param1) || (msg->param0 != msg->param2))
    {
        debugf("Message parameter corrupted\n");
        return;
    }

    message_handler(msg);
}

//-------------------Send debugf()------------------------------------------
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "usbd_cdc_if.h"

void debugf(const char *__restrict format, ...)
{
    // TODO testing on memory savety
    va_list args;
    va_list args_copy;
    char *buffer;
    int buffer_size;

    // Start processing the variable arguments
    va_start(args, format);

    // Copy args to use it twice
    va_copy(args_copy, args);

    // Get the size of the buffer needed
    buffer_size = vsnprintf(NULL, 0, format, args) + 1; // +1 for the END_OF_TRAMISSION_MARKER

    // Allocate the buffer dynamically
    buffer = (char *)malloc(buffer_size + 1);
    if (buffer == NULL)
    // Handle memory allocation failure
    {
        va_end(args);
        va_end(args_copy);
        return;
    }

    // Format the string
    vsnprintf(buffer, buffer_size, format, args_copy);

    buffer[buffer_size - 1] = END_OF_TRAMISSION_MARKER;

// Send the formatted string
#if CONSOLE_MODE == CONSOLE_MODE_UART
    HAL_UART_Transmit(huart_com, (uint8_t *)buffer, buffer_size, HAL_MAX_DELAY);

#elif CONSOLE_MODE == CONSOLE_MODE_VCOM
    const uint8_t RECONNECT_TRYS = 5;
    const uint32_t RECONNECT_TIMEOUT = 5;

    uint8_t status = 1;
    uint8_t trys = 0;
    do
    {
        HAL_Delay(RECONNECT_TIMEOUT);
        status = CDC_Transmit_FS((uint8_t *)buffer, (uint16_t)buffer_size); // Send data via USB}while(status != USBD_OK)
        trys++;
        if (trys > RECONNECT_TRYS)
        {
            break;
        }
    } while (status == USBD_BUSY);

#endif

    // Clean up
    free(buffer);
    va_end(args);
    va_end(args_copy);
}

//----------------------Software Bootjumprt-------------------------------------

/* USER CODE BEGIN 4 */
#define BOOT_ADDR 0x1FFF6FFE // 0x1FFFF000 // my MCU boot code base address s. https://www.st.com/resource/en/application_note/an2606-stm32-microcontroller-system-memory-boot-mode-stmicroelectronics.pdf page 41
#define MCU_IRQS 61u         // no. of NVIC IRQ inputs. s.https://www.st.com/resource/en/datasheet/stm32l432kc.pdf page 34

struct boot_vectable_
{
    uint32_t Initial_SP;
    void (*Reset_Handler)(void);
};

#define BOOTVTAB ((struct boot_vectable_ *)BOOT_ADDR)

/**
 * This restarts the uC as DFU Programmer.
 * You can then flash it via CubeProgrammer or dfu-util
 * It basicly mimics the reload and boot0 = HIGH
 */
void JumpToBootloader(void)
{
    /*Disable all perithals*/
    extern ADC_HandleTypeDef hadc1;
    extern I2C_HandleTypeDef hi2c1;

    HAL_I2C_DeInit(&hi2c1);
    HAL_ADC_DeInit(&hadc1);

    // Reset USB
    USB->CNTR = 0x0003;

    /* Disable all interrupts */
    __disable_irq();

    /* Disable Systick timer */
    SysTick->CTRL = 0;

    /* Set the clock to the default state */
    HAL_RCC_DeInit();

    /* Clear Interrupt Enable Register & Interrupt Pending Register */
    for (uint8_t i = 0; i < (MCU_IRQS + 31u) / 32; i++)
    {
        NVIC->ICER[i] = 0xFFFFFFFF;
        NVIC->ICPR[i] = 0xFFFFFFFF;
    }

    /* Re-enable all interrupts */
    __enable_irq();

    // Set the MSP
    __set_MSP(BOOTVTAB->Initial_SP);

    // Jump to app firmware
    BOOTVTAB->Reset_Handler();
}

//-------------------------Extra-----------------

/**
 * a pretty startupmessage to enjoy bugfixing
 */
void print_startup_message(void)
{
    debugf("\033[0m\033[31m\n___________           __________                __________  _____________ ___ \n");
    debugf("\033[0m\033[31m\\_   _____/___ ___  __\\______   \\_____  ___.__. \\______   \\/   _____/    |   \\\n");
    debugf("\033[0m\033[31m |    __)/ __ \\\\  \\/  /|       _/\\__  <   |  |  |     ___/\\_____  \\|    |   /\n");
    debugf("\033[0m\033[31m |     \\\\  ___/ >    < |    |   \\ / __ \\\\___  |  |    |    /        \\    |  / \n");
    debugf("\033[0m\033[31m \\___  / \\___  >__/\\_ \\|____|_  /(____  / ____|  |____|   /_______  /______/\n");
    debugf("\033[0m\033[31m     \\/      \\/      \\/       \\/      \\/\\/                        \\/          \n");
    debugf("\033[0m\033[31m.[High Voltage Side] Firmware V1.0\n");
    debugf("\033[0m\033[31m.!Keep in mind, Gate and Optik0 are wrongly labled!\n.\n");
}

/**
 * Small debug Function wich prints out memory in hex
 */
void print_bytes(void *ptr, unsigned int size)
{
    uint8_t *byte_ptr = (uint8_t *)ptr; // Cast to a byte pointer for iteration
    debugf("Bytes at memory location %p:\n(hex)", ptr);

    for (unsigned int i = 0; i < size; i++)
    {
        debugf("|%02X", (unsigned int)byte_ptr[i]); // Correct: Cast to unsigned int
    }
    debugf("| (%ibytes)\n", size);
}

void toggle_status_LED(GPIO_TypeDef *GPIO_LED, uint16_t GPIO_Pin_LED)
{
    const uint32_t delay = 1000;
    static uint32_t timestamp = 0;

    uint32_t current_time = HAL_GetTick();
    if (current_time > timestamp)
    {
        HAL_GPIO_TogglePin(GPIO_LED, GPIO_Pin_LED);
        timestamp = current_time + delay;
    }
}
