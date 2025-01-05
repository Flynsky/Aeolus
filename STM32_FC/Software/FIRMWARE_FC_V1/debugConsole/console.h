#ifndef CONSOLE_H
#define CONSOLE_H

#include "main.h"

// ---------------settting up-----------
//  in usbd_cdc_if.c 
// comment out:
//  1.
//  static int8_t CDC_Receive_FS(uint8_t* pbuf, uint32_t *Len);
//  2:
//  static int8_t CDC_Receive_FS(uint8_t* Buf, uint32_t *Len)
//  {
//    /* USER CODE BEGIN 6 */
//    USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &Buf[0]);
//    USBD_CDC_ReceivePacket(&hUsbDeviceFS);
//    return (USBD_OK);
//    /* USER CODE END 6 */
//  }
// include console.h

//-----------------------hardware------------------------
/**
 * This console can be accessed via COM and UART.(s. CONSOLE_MODE)
 *
 * Upload:
 * Uses struct message to upload commands and parameters to the uC.
 * Each message gets terminated with a END_OF_TRAMISSION_MARKER
 *
 * Download:
 * Uses struct packet as fixed data container, debugf() for variable length string based messages.
 * To differenciate bewteen the different download types, packages are starting with 3x PACKAGE_SPECIFIER before the actual data gets send.
 *
 * Requires to function:
 * console_init() call
 * the right UART enabled  with DMA(normal mode) on RX when using Uart
 * USB + Virtual COM Port enabled when using VCOM
 */

#define CONSOLE_MODE_VCOM 0 // needs a virtual com port
#define CONSOLE_MODE_UART 1 // needs a UART and DMA on UART_RX

#define CONSOLE_MODE CONSOLE_MODE_VCOM // here the CONSOLE_MODE can be changed between Virtual Com Port [COM] and Serial Communciation [UART] for recieving messages

#if CONSOLE_MODE == CONSOLE_MODE_UART
// #include "../main.c"
extern UART_HandleTypeDef huart2;
// UART_HandleTypeDef *huart_com = &huart1;
UART_HandleTypeDef *huart_com = &huart2;
#endif

#if CONSOLE_MODE == CONSOLE_MODE_VCOM
extern USBD_HandleTypeDef hUsbDeviceFS;
extern uint8_t VCP_Buffer_rx[]; // buffer of incommming rx data aka messages
void console_check_for_messages();
int8_t CDC_Receive_FS(uint8_t *pbuf, uint32_t *Len);
#endif

//----------------header-------------------------------------------

extern const char END_OF_TRAMISSION_MARKER; // EOT character 0x04
extern const uint8_t PACKAGE_SPECIFIER;     // this gets added 3x in front of the package to different it from debug data

typedef enum messageTypes
{
    NODATA_AVALIABLE = 0, // gets used in buffers. NO NOT USE FOR COMMAND. it will be ignored.
    CTRL_HELP = (int)'?',
    CTRL_SEND_TEST_PACKET = (int)'p',
    CTRL_RESTART = (int)'r',
    CTRL_I2C_SCAN = (int)'s',
    CTRL_BOOT = (int)'h'
} messageTypes;

/*used to control the Highside uC via Usb from the Lowside uC*/
struct message
{
    char command0; // type 0,1 & 3 are identical to catch uplinkerrors
    char command1;
    char command2;
    char command3;
    float param0; // param 0,1 & 3 are identical to catch uplinkerrors
    float param1;
    float param2;
};

/*data structure to stream data from the Highside uC to the Lowside uC to the PC*/
struct packet
{
    unsigned int timestamp; // Timestamp of packet creation in ms
    float v_gate_set;       // the desired voltage
    float v_optik0_set;     // the desired voltage
    float v_optik1_set;     // the desired voltage

    float v_gate_mesure;     // the measured voltage
    float v_optik0_measure;  // the measured voltage
    float v_optik1_measure;  // the measured voltage
    float i_emittor_measure; // the measured current

    float v_hv_mesure;     // the measured voltage
    float i_anode_measure; // the measured curre
};


void console_init(void);
void debugf(const char *__restrict format, ...);
void print_startup_message(void);
void toggle_status_LED(GPIO_TypeDef *GPIO_LED, uint16_t GPIO_Pin_LED);



#endif