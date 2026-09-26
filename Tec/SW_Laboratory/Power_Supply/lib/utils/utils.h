#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stddef.h>
#include "ch32v00x.h"

// I2C low-level
void i2c_init(uint32_t freq);

uint8_t i2c_probe(uint8_t addr);

uint8_t i2c_write_reg_8(uint8_t addr, uint8_t reg, uint8_t value);
uint8_t i2c_read_reg_8(uint8_t addr, uint8_t reg, uint8_t *value);

uint8_t i2c_write_reg_16(uint8_t addr, uint8_t reg, uint16_t value);
uint8_t i2c_read_reg_16(uint8_t addr, uint8_t reg, uint16_t *value);

// SPI
void spi_init(void);

#endif