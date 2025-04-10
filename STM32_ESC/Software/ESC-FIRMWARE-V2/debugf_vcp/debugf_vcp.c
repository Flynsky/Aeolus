/**
 * Adds debugf for STM32.
 */

#include "debugf_vcp.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "usbd_cdc_if.h"

#include "usbd_core.h"

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

//software-triggert DFU

extern USBD_HandleTypeDef hUsbDeviceFS;
extern USBD_DescriptorsTypeDef FS_Desc;

#include "stm32l4xx.h"

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
