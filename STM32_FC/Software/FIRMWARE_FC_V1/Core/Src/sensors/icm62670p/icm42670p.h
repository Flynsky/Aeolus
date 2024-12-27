#ifndef ICM42670P_H
#define ICM42670P_H

extern SPI_HandleTypeDef hspi2;
SPI_HandleTypeDef *hspi_icm = &hspi2;

const uint32_t SPI_TIMEOUT_ICM = 100;

enum ICM_CONFIG
{
    ACCEL_MODE_LOWPOWER = 0,
    ACCEL_MODE_LOWNOISE = 1,

    GYRO_DEG_250 = 250,   // sensitifit in degres per second. (binary: 01100000))(hex:0x60)
    GYRO_DEG_500 = 500,   // sensitifit in degres per second. (binary: 01000000))(hex:0x40)
    GYRO_DEG_1000 = 1000, // sensitifit in degres per second. (binary: 00100000))(hex:0x20)
    GYRO_DEG_2000 = 2000, // sensitifit in degres per second. (binary: 00000000))(hex:0x00)

    ACCEL_G_2 = 2,  // sensitifit in g-force per second. (binary: 01100000)(hex:0x60)
    ACCEL_G_4 = 4,  // sensitifit in g-force per second. (binary: 01000000)(hex:0x40)
    ACCEL_G_8 = 8,  // sensitifit in g-force per second. (binary: 00100000)(hex:0x20)
    ACCEL_G_16 = 16, // sensitifit in g-force per second. (binary: 00000000)(hex:0x00)

    ICM_ODR_1_6kHz = 0x05, // Output Data Rate 1.6kHz (binary: 000000101)
    ICM_ODR_400Hz = 0x1F,  // Output Data Rate 400Hz (binary: 00000111)
    ICM_ODR_100Hz = 0x09,  // Output Data Rate 100Hz (binary: 00001001)
    ICM_ODR_1_5Hz = 0x0F,  // Output Data Rate 1.5Hz (binary: 00001111)

} ICM_CONFIG;

const uint8_t ACCEL_MODE = ACCEL_MODE_LOWPOWER; // 1 = LOW
const int GYRO_DEG_PER_SECOND_MAX = GYRO_DEG_1000;
const int ACCEL_G_PER_SECOND_MAX = ACCEL_G_8;
const uint8_t ICM_ODR = (uint8_t)ICM_ODR_400Hz;

void icm_init();
void icm_read_reg(const uint8_t reg_adress, uint8_t *taget_adress, uint16_t size);
void icm_write_reg(const uint8_t reg_adress, const uint8_t data);

struct icm_data *icm_read_data(void);
void icm_print_data(struct icm_data *data);

struct icm_data
{
    float ACCEL_DATA_X;
    float ACCEL_DATA_Y;
    float ACCEL_DATA_Z;

    float GYRO_DATA_X;
    float GYRO_DATA_Y;
    float GYRO_DATA_Z;

    float DEG_X;
    float DEG_Y;
    float DEG_Z;

    float POS_X;
    float POS_Y;
    float POS_Z;

    float TEMP;
    uint32_t TIMESTAMP
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