#ifndef DEBUG_C
#define DEBUG_C

#include "main.h"
#include <stdio.h>    
#include "usb_device.h"
#include "usbd_cdc_if.h"

int _write(int file, char *data, int len);

int _write(int file, char *data, int len) {
    (void)file;
    CDC_Transmit_FS((unsigned char *)data, len); // Send data over USB CDC
    return len; // Indicate how many bytes were written
}

#endif