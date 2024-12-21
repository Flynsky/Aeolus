/**
 * embedded driver for the ICM-42670-P
 * website: https://invensense.tdk.com/products/motion-tracking/6-axis/icm-42670-p/
 * data sheet: https://invensense.tdk.com/wp-content/uploads/2021/07/ds-000451_icm-42670-p-datasheet.pdf
 */

#include "main.h"
#include "icm42670p.h"
#include "../../debug.c"

static void icm_CS(uint8_t status);

void icm_init()
{
    icm_read_reg(117, 1);
}

void icm_CS(uint8_t status)
{
    if (status == 0)
    {
        HAL_GPIO_WritePin(PIN_IMU_CS_GPIO_Port, PIN_IMU_CS_Pin, GPIO_PIN_RESET);
    }
    else
    {
        HAL_GPIO_WritePin(PIN_IMU_CS_GPIO_Port, PIN_IMU_CS_Pin, GPIO_PIN_SET);
    }
}

uint8_t *icm_read_reg(uint8_t reg_adress, uint16_t size)
{
    // sets first bit to indicade reading
    reg_adress = reg_adress | 0b10000000;

    // send read command
    icm_CS(0);
    HAL_StatusTypeDef ret = HAL_SPI_Transmit(hspi_icm, &reg_adress, 1, 100);
    icm_CS(1);

    switch (ret)
    {
    case HAL_OK:
        // debugf("icm SPI write:HAL_OK\n");
        break;
    case HAL_ERROR:
        debugf("icm SPI write:HAL_ERROR\n");
        break;
    case HAL_BUSY:
        debugf("icm SPI write:HAL_BUSY\n");
        break;
    case HAL_TIMEOUT:
        debugf("icm SPI write:HAL_TIMEOUT\n");
        break;
    default:
        break;
    }

    // prepares buffer for answer
    uint8_t *buffer = (uint8_t *)malloc(size);
    HAL_Delay(10);

    // reads answer
    icm_CS(0);
    HAL_StatusTypeDef ret2 = HAL_SPI_Receive(hspi_icm, buffer, size, 100);
    icm_CS(1);

    switch (ret2)
    {
    case HAL_OK:
        // debugf("icm SPI read:HAL_OK\n");
        break;
    case HAL_ERROR:
        debugf("icm SPI read:HAL_ERROR\n");
        break;
    case HAL_BUSY:
        debugf("icm SPI read:HAL_BUSY\n");
        break;
    case HAL_TIMEOUT:
        debugf("icm SPI read:HAL_TIMEOUT\n");
        break;
    default:
        break;
    }

    // printf result
    reg_adress = reg_adress & 0b01111111;
    debugf("register %i reads: 0x", reg_adress);
    for (int i = 0; i < size; i++)
    {
        debugf(" %x", buffer[i]);
    }
    debugf("\n");

    return buffer;
}

void icm_write_reg(uint8_t reg_adress, uint8_t *data, uint8_t data_size)
{
    uint8_t buffer[data_size + 1];
    buffer[0] = reg_adress;
    for (int i = 0; i < data_size; i++)
    {
        buffer[i + 1] = data[i];
    }

    HAL_StatusTypeDef ret = HAL_SPI_Transmit(hspi_icm, buffer, data_size + 1, 100);

    switch (ret)
    {
    case HAL_OK:
        // debugf("SPI:HAL_OK\n");
        break;
    case HAL_ERROR:
        debugf("SPI:HAL_ERROR\n");
        break;
    case HAL_BUSY:
        debugf("SPI:HAL_BUSY\n");
        break;
    case HAL_TIMEOUT:
        debugf("SPI:HAL_TIMEOUT\n");
        break;

    default:
        break;
    }
}