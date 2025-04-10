/**
 * Adds debugf for STM32.
 */

#include "debugf_vcp.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "usbd_cdc_if.h"

/**
 * prints formated message over CDC
 */
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
    buffer_size = vsnprintf(NULL, 0, format, args) + 1;

    // Allocate the buffer dynamically
    buffer = (char *)malloc(buffer_size);
    if (buffer == NULL)
    // Handle memory allocation failure
    {
        va_end(args);
        va_end(args_copy);
        return;
    }

    // Format the string
    vsnprintf(buffer, buffer_size, format, args_copy);

    // buffer[buffer_size - 1] = '\n';

    // Send the formatted string
    const uint8_t RECONNECT_TRYS = 5;
    const uint32_t RECONNECT_TIMEOUT = 5;

    uint8_t status = 1;
    uint8_t trys = 0;
    do
    {
        HAL_Delay(RECONNECT_TIMEOUT);
        status = CDC_Transmit_FS((uint8_t *)buffer, (uint16_t)buffer_size - 1); // Send data via USB}while(status != USBD_OK)
        trys++;
        if (trys > RECONNECT_TRYS)
        {
            break;
        }
    } while (status == USBD_BUSY);

    // Clean up
    free(buffer);
    va_end(args);
    va_end(args_copy);
}

extern uint8_t UserRxBufferFS[];
/**
 * Checks CDC recive buffer for new data.
 * Works with no modifications of any other files, just call this message periodicly.
 */
void console_check()
{
    /*prevents ghost message recivement if by chance 0 is in buffer*/
    static unsigned char init = 0;
    if (!init)
    {
        UserRxBufferFS[0] = 0;
        init = 1;
    }

    /*checks if 0 is overridden -> new message in buffer*/
    if (UserRxBufferFS[0] != 0)
    {
        debugf("Received data: %s\n", UserRxBufferFS);

        /*decode message*/
        char command[4];                                      // Store the command (e.g., "/C")
        float param0 = 0, param1 = 0, param2 = 0, param3 = 0; // Up to 4 parameters

        /*Use sscanf to extract the command and up to 4 floats*/
        int num_params = sscanf((const char *)UserRxBufferFS, "%s %f %f %f %f", command, &param0, &param1, &param2, &param3);
        (void)num_params; // debugf("num_params:%i\n", num_params);

        // Print the command and the parameters
        debugf("Comand:%s|%f|%f|%f|%f|\b", command, param0, param1, param2, param3);

        // debugf("r:%i,n:%i\n",com_encoded, (int)('d' << 24 | 'f' << 16 | 'u' << 8 | 0));
        int com_encoded = (int)((command[0] << 24) | (command[1] << 16) | (command[2] << 8) | command[3]);
        switch (com_encoded)
        {
        case (int)('d' << 24 | 'f' << 16 | 'u' << 8 | 0):
            debugf("\n--DFU update--\n");
            HAL_Delay(10);
            jump_to_dfu_bootloader();
            break;

        default:
            break;
        }

        UserRxBufferFS[0] = 0;
    }
}

#include "stm32l4xx.h"
#include "usbd_core.h"

extern USBD_HandleTypeDef hUsbDeviceFS;
extern USBD_DescriptorsTypeDef FS_Desc;

/**
 * Trigger DFU bootloader via Software
 */
void jump_to_dfu_bootloader()
{
    USBD_Stop(&hUsbDeviceFS);
    USBD_DeInit(&hUsbDeviceFS);

    // Disable all interrupts
    __disable_irq();

    // Reset USB peripheral (optional, but good practice)
    RCC->APB1ENR1 &= ~RCC_APB1ENR1_USBFSEN;
    RCC->APB1ENR1 |= RCC_APB1ENR1_USBFSEN;

    // Set the vector table MSP and jump to bootloader
    uint32_t bootloader_address = 0x1FFF0000; // STM32L4 system memory

    __set_MSP(*(volatile uint32_t *)bootloader_address);
    void (*bootloader_jump)(void) = (void (*)(void))(*(volatile uint32_t *)(bootloader_address + 4));
    bootloader_jump();
}
