#ifndef ICM42670P_H
#define ICM42670P_H

extern SPI_HandleTypeDef hspi2;
SPI_HandleTypeDef * hspi_icm = &hspi2;

void icm_init();
uint8_t *icm_read_reg(uint8_t reg_adress, uint16_t size);

typedef enum icm_register{
    DEVICE_CONFIG = 0x04
}icm_register;

#endif