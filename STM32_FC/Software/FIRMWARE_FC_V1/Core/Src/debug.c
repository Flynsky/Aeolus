// #include "main.h"
// #include "stdio.h"
// #include "usbd_cdc_if.h"
// #include "usbd_cdc.h"

// int _write(int file, char *ptr, int len)
// {
//     uint8_t rev = CDC_Transmit_FS((uint8_t *)ptr, len);

//     // basicly a Flush
//     while (rev == USBD_BUSY)
//     {
//         rev = CDC_Transmit_FS((uint8_t *)ptr, len);
//     }
//     return len;
// }

// // #define debugf printf
