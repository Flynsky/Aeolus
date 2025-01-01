/**
 * Here goes all things concerning the download in packet form
 */
#include "main.h"
#include "console.h"


struct package;

extern float v_gate_set;
extern float v_optik0_set;
extern float v_optik1_set;

struct packet *packet_create();
void packet_destroy(struct packet *pa);
void packet_send(struct packet *pa);
void packet_send_test();

/*creates and fills a new package*/
struct packet *packet_create()
{
    struct packet *newPacket = (struct packet *)malloc(sizeof(struct packet));
    if (!newPacket)
    {
        // ToDo Error Handeling
        //    return;
    }
    newPacket->timestamp = HAL_GetTick();
    return newPacket;
}

void packet_destroy(struct packet *pa)
{
    free(pa);
}

/**
 * sends a package via USART.
 * Adds 3 times PACKAGE_SPECIFIER in front of the byte stream so it gets recognised as package
 */
void packet_send(struct packet *pa)
{
    // fills buffer with 3x PACKAGE_SPECIFIER bit and 1x package
    int txbuffer_size = 3 + sizeof(struct packet) + 1;
    unsigned char *txBuffer = (unsigned char *)malloc(txbuffer_size);
    memcpy(txBuffer + 3, pa, sizeof(struct packet));
    txBuffer[0] = PACKAGE_SPECIFIER;
    txBuffer[1] = PACKAGE_SPECIFIER;
    txBuffer[2] = PACKAGE_SPECIFIER;
    txBuffer[txbuffer_size - 1] = END_OF_TRAMISSION_MARKER;
    // send it
    // ToDo
#if CONSOLE_MODE == CONSOLE_MODE_UART
    HAL_UART_Transmit(huart_com, (uint8_t *)txBuffer, txbuffer_size, HAL_MAX_DELAY);

#elif CONSOLE_MODE == CONSOLE_MODE_VCOM
    CDC_Transmit_FS((uint8_t *)txBuffer, (uint16_t)txbuffer_size); // Send data via USB
#endif

    free(txBuffer);
    free(pa);
}

/*sends a test packet with 0, 1, 2, 3, 4*/
void packet_send_test()
{
    // creates test packet
    struct packet *test_packet = packet_create();

    // fills with test values
    // test_packet->timestamp = 0;
    test_packet->v_gate_set = 1.0;
    test_packet->v_optik0_set = 2.0;
    test_packet->v_optik1_set = 3.0;
    test_packet->v_gate_mesure = 4.0;
    test_packet->v_optik0_measure = 5.0;
    test_packet->v_optik1_measure = 6.0;
    test_packet->i_emittor_measure = 7.0;

    // send it
    packet_send(test_packet);
}