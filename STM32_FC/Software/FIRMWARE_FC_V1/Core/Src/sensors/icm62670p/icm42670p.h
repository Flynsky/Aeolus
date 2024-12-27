#ifndef ICM42670P_H
#define ICM42670P_H

extern SPI_HandleTypeDef hspi2;
SPI_HandleTypeDef *hspi_icm = &hspi2;

const uint32_t SPI_TIMEOUT_ICM = 100;

void icm_init();
void icm_read_reg(const uint8_t reg_adress,const  uint8_t *taget_adress,const  uint8_t size);
void icm_write_reg(const uint8_t reg_adress, const uint8_t data);


struct icm_data *icm_read_data(void);

struct icm_data
{
    int16_t ACCEL_DATA_X;
    int16_t ACCEL_DATA_Y;
    int16_t ACCEL_DATA_Z;

    int16_t GYRO_DATA_X;
    int16_t GYRO_DATA_Y;
    int16_t GYRO_DATA_Z;

    uint16_t TEMP;
    uint16_t TIMESTAMP;
};

typedef enum icm_userbank_register
{
    DEVICE_CONFIG = 4,
    INT_CONFIG = 6,

    TEMP_DATA1 = 9,  // Upper byte of temperature data
    TEMP_DATA0 = 10, // Lower byte of temperature data

    ACCEL_DATA_X1 = 11, // Upper byte of Accel X-axis data
    ACCEL_DATA_X0 = 12, // Lower byte of Accel X-axis data
    ACCEL_DATA_Y1 = 13, // Upper byte of Accel Y-axis data
    ACCEL_DATA_Y0 = 14, // Lower byte of Accel Y-axis data
    ACCEL_DATA_Z1 = 15, // Upper byte of Accel Z-axis data
    ACCEL_DATA_Z0 = 16, // Lower byte of Accel Z-axis data

    GYRO_DATA_X1 = 17, // Upper byte of GYRO X-axis data
    GYRO_DATA_X0 = 18, // Lower byte of GYRO X-axis data
    GYRO_DATA_Y1 = 19, // Upper byte of GYRO Y-axis data
    GYRO_DATA_Y0 = 20, // Lower byte of GYRO Y-axis data
    GYRO_DATA_Z1 = 21, // Upper byte of GYRO Z-axis data
    GYRO_DATA_Z0 = 22, // Lower byte of GYRO Z-axis data

    PWR_MGMT0 = 31,     // 3:2 GYRO_MODE 00 off 01 standby 11 low noise
                        // 1:0 ACCEL_MODE 00 off 01 off 10 low power 11 low noise
    GYRO_CONFIG0 = 32,  // 6:5 GYRO_SACLE 00:2000 01:1000 10:500 11:250dps
                        // 3:0 Gyroscope ODR (OUTPUT data rate) 0101:1.6kHz 0111:400Hz 1010:50Hz
    ACCEL_CONFIG0 = 33, // 6:5 ACCEL_SACLE 00 16g 01 8g 10 4g 11 2g
                        // 3:0 ACCEL_ODR (OUTPUT data rate) 0101:1.6kHz 0111:400Hz 1010:50Hz

    TEMP_CONFIG0 = 34,  // bandwith temerature
    GYRO_CONFIG1 = 35,  // low pass gyro
    ACCEL_CONFIG1 = 36, // 6:4 ACEL Averaging
                        // 2:0 ACELL Lowpass
    FIFO_CONFIG1 = 40,  // 1 FIFO_MODE
                        // 0 FIFO_BYPASS
    FIFO_COUNTH = 61,   // High Bits, count indicates the number of records or bytes available in FIFO
                        // according to FIFO_COUNT_FORMAT setting.
                        // Note: Must read FIFO_COUNTL to latch new data for both FIFO_COUNTH
                        // and FIFO_COUNTL.
    FIFO_COUNTL = 62,   // Low Bits, count indicates the number of records or bytes available in FIFO
                        // according to FIFO_COUNT_REC setting.
                        // Reading this byte latches the data for both FIFO_COUNTH, and
    FIFO_DATA = 63,     // FIFO data por
    WHO_AM_I = 117,     // Register to indicate to user which device is being accessed. default 0x67

    BLK_SEL_W = 121, // Block address for accessing MREG1 or MREG2 register space for register write operation
    MADDR_W = 122,   // To write to a register in MREG1 or MREG2 space, set this register field to the
                     // address of the register in MREG1 or MREG2 space
    M_W = 123,       // To write a value to a register in MREG1 or MREG2 space, that value must be written to M_W.
    BLK_SEL_R = 124, // Block address for accessing MREG1 or MREG2 register space for register read operation
    MADDR_R = 125,   // To read the value of a register in MREG1 or MREG2 space, set this register
                     // field to the address of the register in MREG1 or MREG2 space.
    M_R = 126,       // To read the value of a register in MREG1 or MREG2 space, that value is
                     // accessed from M_R.

} icm_userbank_register;

typedef enum icm_mreg1_register
{
    TMST_CONFIG1 = 0,
    FIFO_CONFIG5 = 1,
    FIFO_CONFIG6 = 2,
    FSYNC_CONFIG = 3,
    INT_CONFIG0 = 4,
    INT_CONFIG1 = 5,
    SENSOR_CONFIG3 = 6,
    ST_CONFIG = 19,
    SELFTEST = 20,

    OFFSET_USER0 = 78, // Lower bits of X-gyro offset programmed by user. Max value is ±64 dps,
                       // resolution is 1/32 dps.
} icm_mreg1_register;

#endif