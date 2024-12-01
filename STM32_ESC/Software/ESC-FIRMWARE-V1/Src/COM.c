#include "main.h"
#include "usb_device.h"
#include <stdio.h>
#include "usbd_cdc_if.h"

void MessageHandler();

struct message
{
    char command0;
    char param0;
    char param1;
    char param2;
    char param3;
};

uint8_t CDC_Receive_FS(uint8_t *Buf, uint32_t *Len)
{
    static union byte_to_message
    {
        struct message msg;
        uint8_t buf[sizeof(struct message)];
    } byteTomsg;

    // check if message is valid

    //
}
