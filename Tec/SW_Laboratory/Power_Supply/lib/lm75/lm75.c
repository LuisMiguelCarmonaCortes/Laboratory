#include <stdio.h>
#include <stdint.h>
#include "lm75.h"
#include "utils.h"


// Funcion auxiliar de debugueo de escritura de 8bits para el registro de configuracion
static lm75_Status_t lm75_WriteConfig(lm75_t *dev)
{
    if (i2c_write_reg_8(dev->i2c_addr, LM75_CONF_REG, dev->config_reg) == 0)
    {
        return LM75_OK;
    }

    return I2C_ERROR;
}


// Función para escribir registros de 16 bits (TOS y THYST)
static lm75_Status_t lm75_WriteConfig_16(lm75_t *dev, uint8_t reg, int16_t value)
{
    if (i2c_write_reg_16(dev->i2c_addr, reg, value) == 0)
    {
        return LM75_OK;
    }

    return I2C_ERROR;
}


lm75_Status_t lm75_Init(lm75_t *dev, uint8_t dir)
{
    dev->i2c_addr = dir;
    // Iniciar todo con 0, por defecto.
    dev->config_reg = 0x00;

    return lm75_WriteConfig(dev);
}


lm75_Status_t lm75_SetFaultQueue(lm75_t *dev, uint8_t value)
{
    // Comprobar que el valor es valido
    if (value > 3) {
        return LM75_INVALID_PARAM;
    }

    // Limpiar bits 3 y 4
    dev->config_reg &= ~LM75_FAULT_QUEUE;

    // Asignar nuevo valor
    dev->config_reg |= (value << 3);  // desplazar a bits 3 y 4

    return lm75_WriteConfig(dev);
}


lm75_Status_t lm75_SetPolarity(lm75_t *dev, uint8_t polarity)
{
    // Comprobar que el valor es valido
    if (polarity > 1) {
        return LM75_INVALID_PARAM;
    }

    // Limpiar bit 2
    dev->config_reg &= ~LM75_POLARITY;

    // Asignar nuevo valor
    dev->config_reg |= (polarity << 2);  // desplazar a bit 2

    return lm75_WriteConfig(dev);
}


lm75_Status_t lm75_SetMode(lm75_t *dev, uint8_t mode)
{
    // Comprobar que el valor es valido
    if (mode > 1) {
        return LM75_INVALID_PARAM;
    }

    // Limpiar bit 1
    dev->config_reg &= ~LM75_MODE;

    // Asignar nuevo valor
    dev->config_reg |= (mode << 1);  // desplazar a bit 1

    return lm75_WriteConfig(dev);
}


lm75_Status_t lm75_SetShutdown(lm75_t *dev, uint8_t shutdown)
{
    // Comprobar que el valor es valido
    if (shutdown > 1) {
        return LM75_INVALID_PARAM;
    }

    // Limpiar bit 0
    dev->config_reg &= ~LM75_SHUTDOWN;

    // Asignar nuevo valor
    dev->config_reg |= (shutdown << 0);  // desplazar a bit 0

    return lm75_WriteConfig(dev);
}


lm75_Status_t lm75_GetConfig(lm75_t *dev)
{
    uint8_t configuracion;
    uint16_t o_temp, hys;

    if (i2c_read_reg_8(dev->i2c_addr, LM75_CONF_REG, &configuracion))
    {
        return I2C_ERROR;
    }

    dev->config_reg = configuracion;

    if (i2c_read_reg_16(dev->i2c_addr, LM75_TOS_REG, &o_temp))
    {
        return I2C_ERROR;
    }

    if (i2c_read_reg_16(dev->i2c_addr, LM75_THYST_REG, &hys))
    {
        return I2C_ERROR;
    }

    // shift FIX (sin float intermedio innecesario)
    o_temp >>= 7;
    hys >>= 7;

    // Datos del LM75
    uint8_t fq = (configuracion & LM75_FAULT_QUEUE) >> 3;

    printf("Fault queue data -> %d\n", fq == 0 ? 1 : fq == 1 ? 2 : fq == 2 ? 4 : 6);
    printf("Polarity -> %s\n", (configuracion & LM75_POLARITY) ? "HIGH" : "LOW");
    printf("Mode -> %s\n", (configuracion & LM75_MODE) ? "Interrupcion" : "Comparacion");
    printf("Shutdown -> %s\n", (configuracion & LM75_SHUTDOWN) ? "OFF" : "ON");

    printf("Over Temp -> %d.%d°C\n", o_temp / 2, (o_temp & 1) ? 5 : 0);
    printf("Histeresis -> %d.%d°C\n", hys / 2, (hys & 1) ? 5 : 0);

    return LM75_OK;
}


lm75_Status_t lm75_read_raw_temp(lm75_t *dev, uint16_t *temp)
{
    if (i2c_read_reg_16(dev->i2c_addr, LM75_TEMP_REG, temp))
    {
        return I2C_ERROR;
    }

    return LM75_OK;
}


lm75_Status_t lm75_read_celsius_temp(lm75_t *dev, float *temp)
{
    uint16_t raw_data;

    lm75_Status_t status = lm75_read_raw_temp(dev, &raw_data);
    if (status != LM75_OK) return status;

    // 5 bit de relleno se quitan
    int16_t temp_int = (int16_t)raw_data >> 5;

    // Cada bit segun datasheet vale 0.125
    *temp = (float)temp_int * 0.125f;

    return LM75_OK;
}


lm75_Status_t lm75_SetThyst(lm75_t *dev, float temp)
{
    // La resolucion es de 9 bit = 0.5ºC
    // La temperatura se divide entre 0.5 o se multiplica por 2
    // Se convierte a int16 y se desplaza 7 posiciones
    int16_t val = (int16_t)(temp * 2.0f) << 7;

    return lm75_WriteConfig_16(dev, LM75_THYST_REG, val);
}


lm75_Status_t lm75_SetTos(lm75_t *dev, float temp)
{
    // La resolucion es de 9 bit = 0.5ºC
    // La temperatura se divide entre 0.5 o se multiplica por 2
    // Se convierte a int16 y se desplaza 7 posiciones
    int16_t val = (int16_t)(temp * 2.0f) << 7;

    return lm75_WriteConfig_16(dev, LM75_TOS_REG, val);
}