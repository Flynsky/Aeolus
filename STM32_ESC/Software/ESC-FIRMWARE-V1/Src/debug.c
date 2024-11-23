
#include "main.h"
#include <stdio.h>    
#include "usb_device.h"
#include "usbd_cdc_if.h"

int _write(int file, char *data, int len);

int _write(int file, char *data, int len) {
    file = 0;
    CDC_Transmit_FS((uint8_t *)data, len); // Send data over USB CDC
    return len; // Indicate how many bytes were written
}
