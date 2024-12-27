/**
 * embedded driver for the ICM-42670-P
 * website: https://invensense.tdk.com/products/motion-tracking/6-axis/icm-42670-p/
 * data sheet: https://invensense.tdk.com/wp-content/uploads/2021/07/ds-000451_icm-42670-p-datasheet.pdf
 */

#include "main.h"
#include "icm42670p.h"
#include "../../debug.c"

struct icm_data;

static void icm_CS(uint8_t status);

void icm_init()
{
    uint8_t buffer;
    // power on ACCEL and Gyro in low Noise mode
    icm_write_reg(PWR_MGMT0, 0b00001111);

    // sets gyro 2000ps and 1.6kHz;
    icm_write_reg(GYRO_CONFIG0, 0b00000101);

    // sets accel 16g and 1.6kHz;
    icm_write_reg(GYRO_CONFIG0, 0b00000101);

    // configs FIFO

    // sends who i am

    icm_read_reg(WHO_AM_I, &buffer, 1);
    debugf("icm42670p 0x%x is initialised\n", buffer);
}

/**
 * sets / clears CS for the ICM
 * @param status SET:1 RESTET:0
 */
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

/**
 * reads out one register.
 * does set CS
 */
void icm_read_reg(const uint8_t reg_adress, const uint8_t *taget_adress, const uint8_t size)
{
    // catch errors
    if (size <= 0)
    {
        debugf("icm SPI read:SIZE<=0\n");
    }

    // sets first bit to indicade reading
    uint8_t reg_adress_bit = reg_adress | 0b10000000; // first bit needs to be placed as a read operator

    // send read command
    icm_CS(0);
    HAL_StatusTypeDef ret = HAL_SPI_Transmit(hspi_icm, &reg_adress_bit, 1, SPI_TIMEOUT_ICM);

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

    // reads answer
    HAL_StatusTypeDef ret2 = HAL_SPI_Receive(hspi_icm, taget_adress, size, SPI_TIMEOUT_ICM);
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
    // debugf("register %i reads: 0x", reg_adress);
    // for (int i = 0; i < size; i++)
    // {
    //     debugf("%x ", taget_adress[i]);
    // }
    // debugf("\n");
}

void icm_write_reg(const uint8_t reg_adress, const uint8_t data)
{
    uint8_t buffer[2];
    buffer[0] = reg_adress;
    buffer[1] = data;

    icm_CS(0);
    HAL_StatusTypeDef ret = HAL_SPI_Transmit(hspi_icm, buffer, 2, SPI_TIMEOUT_ICM);
    icm_CS(1);

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

struct icm_data *icm_read_data(void)
{
    // create container
    struct icm_data *data = (struct icm_data *)malloc(sizeof(struct icm_data));

    // reads data in
    icm_read_reg(ACCEL_DATA_X0, (uint8_t *)&data->ACCEL_DATA_X+1, 1); // lower bytes -> +1 in adress
    icm_read_reg(ACCEL_DATA_X1, (uint8_t *)&data->ACCEL_DATA_X, 1);       // upper byte -> right adress
    icm_read_reg(ACCEL_DATA_Y0, (uint8_t *)(&data->ACCEL_DATA_Y + 1), 1); // lower bytes -> +1 in adress
    icm_read_reg(ACCEL_DATA_Y1, (uint8_t *)&data->ACCEL_DATA_Y, 1);       // upper byte -> right adress
    icm_read_reg(ACCEL_DATA_Z0, (uint8_t *)(&data->ACCEL_DATA_Z + 1), 1); // lower bytes -> +1 in adress
    icm_read_reg(ACCEL_DATA_Z1, (uint8_t *)&data->ACCEL_DATA_Z, 1);       // upper byte -> right adress

    icm_read_reg(GYRO_DATA_X0, (uint8_t *)(&data->GYRO_DATA_X + 1), 1); // lower bytes -> +1 in adress
    icm_read_reg(GYRO_DATA_X1, (uint8_t *)&data->GYRO_DATA_X, 1);       // upper byte -> right adress
    icm_read_reg(GYRO_DATA_Y0, (uint8_t *)(&data->GYRO_DATA_Y + 1), 1); // lower bytes -> +1 in adress
    icm_read_reg(GYRO_DATA_Y1, (uint8_t *)&data->GYRO_DATA_Y, 1);       // upper byte -> right adress
    icm_read_reg(GYRO_DATA_Z0, (uint8_t *)(&data->GYRO_DATA_Z + 1), 1); // lower bytes -> +1 in adress
    icm_read_reg(GYRO_DATA_Z1, (uint8_t *)&data->GYRO_DATA_Z, 1);       // upper byte -> right adress

    icm_read_reg(TEMP_DATA0,(uint8_t *)(&data->TEMP + 1), 1); // lower bytes -> +1 in adress
    icm_read_reg(TEMP_DATA1, (uint8_t *)&data->TEMP, 1);     // upper byte -> right adress

    // Debugfs
    debugf("ACCEL|");
    debugf("X:%i|", data->ACCEL_DATA_X);
    debugf("Y:%i|", data->ACCEL_DATA_Y);
    debugf("Z:%i|\n", data->ACCEL_DATA_Z);

    HAL_Delay(1);

    debugf("GYRO|");
    debugf("X:%i|", data->GYRO_DATA_X);
    debugf("Y:%i|", data->GYRO_DATA_Y);
    debugf("Z:%i|", data->GYRO_DATA_Z);
    debugf("\n");

    return data;
}