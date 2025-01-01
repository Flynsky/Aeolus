#include "main.h"
#include "console.h"

void i2c_scan(I2C_HandleTypeDef *hi2c)
{
    const uint32_t RECONNECTION_TRIALS_I2C = 3;
    const uint32_t TIMEOUT_I2C = 10; // ms

    /*-[ I2C Bus Scanning ]-*/
    debugf(">>Start I2C scan\n");

    // HAL_I2C_DeInit(hi2c);
    // HAL_I2C_Init(hi2c);

    // checks if I2C bus is buisy
    if (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BUSY) == SET)
    {
        debugf("I2C Bus is busy\n");
        return;
    }

    // Proceed with scan
    for (int adress = 1; adress < 128; adress++)
    {

        HAL_StatusTypeDef ret = HAL_I2C_IsDeviceReady(hi2c, (uint16_t)(adress << 1), RECONNECTION_TRIALS_I2C, TIMEOUT_I2C);
        // HAL_Delay(100);

        // uint8_t * data = {1};
        // HAL_StatusTypeDef ret = HAL_I2C_Master_Transmit(hi2c, (uint16_t)(adress << 1),data,1,TIMEOUT_I2C);
        switch (ret)
        {
        case HAL_OK:
            debugf("\033[0m\033[33m -0x%X- \033[0m", adress);
            break;

        case HAL_ERROR:
            debugf(" -ER- ");
            break;

        case HAL_BUSY:
            debugf(" -BUSY- ");
            break;

        case HAL_TIMEOUT:
            debugf(" - ");
            break;

        default:
            debugf(" -UNDEF- ");
            break;
        }
    }

    debugf("\n>>I2C Scan done\n");
}
