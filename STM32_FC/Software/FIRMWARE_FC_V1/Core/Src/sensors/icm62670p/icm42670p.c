/**
 * embedded driver for the ICM-42670-P
 * website: https://invensense.tdk.com/products/motion-tracking/6-axis/icm-42670-p/
 * data sheet: https://invensense.tdk.com/wp-content/uploads/2021/07/ds-000451_icm-42670-p-datasheet.pdf
 */

#include "main.h"
#include "icm42670p.h"
#include "../../debug.c"

extern TIM_HandleTypeDef htim2;
struct icm_data;

static void icm_CS(uint8_t status);

void icm_init()
{
    uint8_t buffer;

    // Power On
    switch (ACCEL_MODE)
    {
    case ACCEL_MODE_LOWNOISE:
        icm_write_reg(PWR_MGMT0, 0b00001111); // ACCEL and Gyro in low Noise mode
        break;

    case ACCEL_MODE_LOWPOWER:
        icm_write_reg(PWR_MGMT0, 0b00001111); // ACCEL in Low power and Gyro in low Noise mode
        break;

    default:
        break;
    }

    // ACCELEROMETER
    switch (ACCEL_G_PER_SECOND_MAX)
    {
    case ACCEL_G_2:
        icm_write_reg(ACCEL_CONFIG0, 0x60 + ICM_ODR); // sets accel range and data output;
        break;

    case ACCEL_G_4:
        icm_write_reg(ACCEL_CONFIG0, 0x40 + ICM_ODR); // sets accel range and data output;
        break;

    case ACCEL_G_8:
        icm_write_reg(ACCEL_CONFIG0, 0x20 + ICM_ODR); // sets accel range and data output;
        break;

    case ACCEL_G_16:
        icm_write_reg(ACCEL_CONFIG0, 0 + ICM_ODR); // sets accel range and data output;
        break;
    default:
        break;
    }

    // icm_write_reg(ACCEL_CONFIG1, 0b01110111); // set accel low pass & averaging
    icm_write_reg(ACCEL_CONFIG1, 0x00);

    // GYRO
    switch (GYRO_DEG_PER_SECOND_MAX)
    {
    case GYRO_DEG_250:
        icm_write_reg(ACCEL_CONFIG0, 0x60 + ICM_ODR); // sets accel range and data output;
        break;

    case GYRO_DEG_500:
        icm_write_reg(ACCEL_CONFIG0, 0x40 + ICM_ODR); // sets accel range and data output;
        break;

    case GYRO_DEG_1000:
        icm_write_reg(ACCEL_CONFIG0, 0x20 + ICM_ODR); // sets accel range and data output;
        break;

    case GYRO_DEG_2000:
        icm_write_reg(ACCEL_CONFIG0, 0x00 + ICM_ODR); // sets accel range and data output;
        break;
    default:
        break;
    }

    // set gyro low pass
    icm_write_reg(GYRO_CONFIG1, 0b00000111);

    // set temp lowpass
    icm_write_reg(TEMP_CONFIG0, 0b01110000);
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
void icm_read_reg(const uint8_t reg_adress, uint8_t *taget_adress, uint16_t size)
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

    // timestamp in us
    static uint32_t last_timestamp = 0;                     // in us
    uint32_t now_timestamp = __HAL_TIM_GET_COUNTER(&htim2); // in us
    data->TIMESTAMP = now_timestamp;                        // perhaos more likeay HAL_gettick()
    uint32_t delta_time;                                    // in us

    if (now_timestamp >= last_timestamp)
    {
        delta_time = now_timestamp - last_timestamp;
    }
    else
    {
        // Handle counter overflow
        const uint32_t nTimerMax = 4294967296 - 1;
        delta_time = (nTimerMax - last_timestamp) + now_timestamp + 1;
    }

    // reads raw data in
    int16_t ACCEL_DATA_X_RAW;
    int16_t ACCEL_DATA_Y_RAW;
    int16_t ACCEL_DATA_Z_RAW;
    icm_read_reg(ACCEL_DATA_X0, (uint8_t *)&ACCEL_DATA_X_RAW + 1, 1); // lower bytes -> +1 in adress
    icm_read_reg(ACCEL_DATA_X1, (uint8_t *)&ACCEL_DATA_X_RAW, 1);     // upper byte -> right adress
    icm_read_reg(ACCEL_DATA_Y0, (uint8_t *)&ACCEL_DATA_Y_RAW + 1, 1); // lower bytes -> +1 in adress
    icm_read_reg(ACCEL_DATA_Y1, (uint8_t *)&ACCEL_DATA_Y_RAW, 1);     // upper byte -> right adress
    icm_read_reg(ACCEL_DATA_Z0, (uint8_t *)&ACCEL_DATA_Z_RAW + 1, 1); // lower bytes -> +1 in adress
    icm_read_reg(ACCEL_DATA_Z1, (uint8_t *)&ACCEL_DATA_Z_RAW, 1);     // upper byte -> right adress

    int16_t GYRO_DATA_X_RAW;
    int16_t GYRO_DATA_Z_RAW;
    int16_t GYRO_DATA_Y_RAW;
    icm_read_reg(GYRO_DATA_X0, (uint8_t *)&GYRO_DATA_X_RAW + 1, 1); // lower bytes -> +1 in adress
    icm_read_reg(GYRO_DATA_X1, (uint8_t *)&GYRO_DATA_X_RAW, 1);     // upper byte -> right adress
    icm_read_reg(GYRO_DATA_Y0, (uint8_t *)&GYRO_DATA_Y_RAW + 1, 1); // lower bytes -> +1 in adress
    icm_read_reg(GYRO_DATA_Y1, (uint8_t *)&GYRO_DATA_Y_RAW, 1);     // upper byte -> right adress
    icm_read_reg(GYRO_DATA_Z0, (uint8_t *)&GYRO_DATA_Z_RAW + 1, 1); // lower bytes -> +1 in adress
    icm_read_reg(GYRO_DATA_Z1, (uint8_t *)&GYRO_DATA_Z_RAW, 1);     // upper byte -> right adress

    uint16_t TEMP_RAW;
    icm_read_reg(TEMP_DATA0, (uint8_t *)&TEMP_RAW + 1, 1); // lower bytes -> +1 in adresS
    icm_read_reg(TEMP_DATA1, (uint8_t *)&TEMP_RAW, 1);     // upper byte -> right adress

    // convert to degree / g per s
    const float nINT_16 = 32768.0 - 1.0; // max size +/- int_16

    data->ACCEL_DATA_X = (float)(ACCEL_DATA_X_RAW * ACCEL_G_PER_SECOND_MAX) / nINT_16;
    data->ACCEL_DATA_Y = (float)(ACCEL_DATA_Y_RAW * ACCEL_G_PER_SECOND_MAX) / nINT_16;
    data->ACCEL_DATA_Z = (float)(ACCEL_DATA_Z_RAW * ACCEL_G_PER_SECOND_MAX) / nINT_16;

    data->GYRO_DATA_X = (float)(GYRO_DATA_X_RAW * GYRO_DEG_PER_SECOND_MAX) / nINT_16;
    data->GYRO_DATA_Y = (float)(GYRO_DATA_Y_RAW * GYRO_DEG_PER_SECOND_MAX) / nINT_16;
    data->GYRO_DATA_Z = (float)(GYRO_DATA_Z_RAW * GYRO_DEG_PER_SECOND_MAX) / nINT_16;

    data->TEMP = ((float)TEMP_RAW / 128.0) + 25.0;

    // debugf(">TEMP_RAW:%d§steps\n", TEMP_RAW);
    // debugf(">ACCEL_X_RAW:%d§steps\n", ACCEL_DATA_X_RAW);
    // debugf(">ACCEL_X_FLOAT:%f§oi\n", data->ACCEL_DATA_X);

    // calculate velocity
    static float VELO_X = 0;
    static float last_acc_x = 0;
    VELO_X += 1000000.0 * ((float)data->ACCEL_DATA_X - last_acc_x) /(float) delta_time; // numerical integration. * 1000000 because deltatime is in us

    // calculate position
    static float POS_X = 0;
    static float last_velo_x = 0;

    POS_X += 1000000.0 * (VELO_X - last_velo_x) / (float)delta_time; // numerical integration.  * 1000000 because deltatime is in us
    data->POS_X = POS_X;

    // calculate rotation
    static float DEG_X = 0;
    static float last_gyro_x = 0;

    DEG_X += (data->GYRO_DATA_X - last_gyro_x) /(float) delta_time; // numerical integration
    data->DEG_X = DEG_X;

    // debugf in the FREQ_PRINT Frequency
    const float FREQ_PRINT = 5;             // in Hz
    int T_PRINT = (int)1000.0 / FREQ_PRINT; // in ms

    static uint32_t last_print = 0;
    if (last_print < HAL_GetTick())
    {
        last_print = HAL_GetTick() + T_PRINT;
        icm_print_data(data);
        debugf(">VELO_X:%f§m/s\n", VELO_X);
        debugf(">IMU_FREQ:%.2f§Hz\n", 1000000.0/ (float)delta_time);
    }

    // reset last variables
    last_timestamp = now_timestamp;
    last_acc_x = (float)data->ACCEL_DATA_X;
    last_velo_x = VELO_X;
    last_gyro_x = data->GYRO_DATA_X;

    return data;
}

void icm_print_data(struct icm_data *data)
{
    // Format the sensor data as key=value pairs
    // debugf(
    //     ">ACCEL_X:%f§g/s\n"
    //     ">ACCEL_Y:%.2f§g/s\n"
    //     ">ACCEL_Z:%.2f§g/s\n"
    //     ">GYRO_X:%f§deg/s\n"
    //     ">GYRO_Y:%.2f§deg/s\n"
    //     ">GYRO_Z:%.2f§deg/s\n"
    //     ">TEMP:%.0f§°C\n"
    //     ">POS_X:%f§mm\n"
    //     ">DEG_X:%.2f§deg\n",
    //     data->ACCEL_DATA_X,
    //     data->ACCEL_DATA_Y,
    //     data->ACCEL_DATA_Z,
    //     data->GYRO_DATA_X,
    //     data->GYRO_DATA_Y,
    //     data->GYRO_DATA_Z,
    //     data->TEMP,
    //     data->POS_X * 1000,
    //     data->DEG_X);

    debugf(">ACCEL_X: %f§g/s\n", data->ACCEL_DATA_X);
    // debugf(">ACCEL_Y: %.2f§g/s\n", data->ACCEL_DATA_Y);
    // debugf(">ACCEL_Z: %.2f§g/s\n", data->ACCEL_DATA_Z);
    // debugf(">GYRO_X: %f§deg/s\n", data->GYRO_DATA_X);
    // debugf(">GYRO_Y: %.2f§deg/s\n", data->GYRO_DATA_Y);
    // debugf(">GYRO_Z: %.2f§deg/s\n", data->GYRO_DATA_Z);
    // debugf(">TEMP: %.0f§°C\n", data->TEMP);
    debugf(">POS_X: %.3f§mm\n", data->POS_X * 1000); // Multiply POS_X by 1000 to convert to mm
    // debugf(">DEG_X: %.2f§deg\n", data->DEG_X);
}
