#include "utils.h"
#include <stdio.h>

#define I2C_TIMEOUT_MS 1000

// Funcon para ver si responde el dispositivo
uint8_t i2c_probe(uint8_t addr)
{
    uint32_t timeout;

    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

    I2C_GenerateSTART(I2C1, ENABLE);

    timeout = 100000;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(--timeout == 0)
            return 0;
    }

    I2C_Send7bitAddress(
        I2C1,
        addr << 1,
        I2C_Direction_Transmitter);

    timeout = 100000;

    while(timeout--)
    {
        uint16_t sr1 = I2C1->STAR1;

        if(sr1 & I2C_STAR1_ADDR)
        {
            volatile uint16_t dummy;

            dummy = I2C1->STAR1;
            dummy = I2C1->STAR2;
            (void)dummy;

            I2C_GenerateSTOP(I2C1, ENABLE);

            return 1;
        }

        if(sr1 & I2C_STAR1_AF)
        {
            I2C1->STAR1 &= ~I2C_STAR1_AF;

            I2C_GenerateSTOP(I2C1, ENABLE);

            return 0;
        }
    }

    I2C_GenerateSTOP(I2C1, ENABLE);

    return 0;
}

// Funcion generica para escribir registro de 8 bits
uint8_t i2c_write_reg_8(uint8_t addr, uint8_t reg, uint8_t value)
{
    uint32_t timeout;

    // Esperar bus libre
    timeout = I2C_TIMEOUT_MS * 1000;
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
        if(--timeout == 0) return 1;

    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C1, addr << 1, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    I2C_SendData(I2C1, reg);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_SendData(I2C1, value);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_GenerateSTOP(I2C1, ENABLE);

    return 0;
}


// Funcion generica para leer registro de 8 bits
uint8_t i2c_read_reg_8(uint8_t addr, uint8_t reg, uint8_t *value)
{
    uint32_t timeout;

    printf("1\r\n");

    // START
    I2C_GenerateSTART(I2C1, ENABLE);

    timeout = 1000000;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(--timeout == 0) return 1;
    }
    printf("2\r\n");

    // SLA + W  (⚠️ SIN << 1)
    I2C_Send7bitAddress(I2C1, addr << 1, I2C_Direction_Transmitter);

    timeout = 1000000;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if(--timeout == 0) return 2;
    }
    printf("3\r\n");

    // Register
    I2C_SendData(I2C1, reg);

    timeout = 1000000;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if(--timeout == 0) return 3;
    }
    printf("4\r\n");

    // Repeated START
    I2C_GenerateSTART(I2C1, ENABLE);

    timeout = 1000000;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(--timeout == 0) return 4;
    }
    printf("5\r\n");

    // SLA + R (⚠️ SIN << 1)
    I2C_Send7bitAddress(I2C1, addr << 1, I2C_Direction_Receiver);

    timeout = 1000000;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        if(--timeout == 0) return 5;
    }
    printf("6\r\n");

    // Solo 1 byte
    I2C_AcknowledgeConfig(I2C1, DISABLE);
    I2C_GenerateSTOP(I2C1, ENABLE);

    timeout = 1000000;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))
    {
        if(--timeout == 0) return 6;
    }

    *value = I2C_ReceiveData(I2C1);

    I2C_AcknowledgeConfig(I2C1, ENABLE);

    return 0;
}


// Funcion generica para escribir registro de 16 bits
uint8_t i2c_write_reg_16(uint8_t addr, uint8_t reg, uint16_t value)
{
    uint32_t timeout;

    // Esperar bus libre
    timeout = I2C_TIMEOUT_MS * 1000;
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
        if(--timeout == 0) return 1;

    // START
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    // Dirección + WRITE
    I2C_Send7bitAddress(I2C1, addr << 1, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    // Registro
    I2C_SendData(I2C1, reg);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    // MSB
    I2C_SendData(I2C1, (uint8_t)(value >> 8));
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    // LSB
    I2C_SendData(I2C1, (uint8_t)(value & 0xFF));
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    // STOP
    I2C_GenerateSTOP(I2C1, ENABLE);

    return 0;
}


// Funcion generica para leer registro de 16 bits
uint8_t i2c_read_reg_16(uint8_t addr, uint8_t reg, uint16_t *value)
{
    uint8_t msb, lsb;

    if(value == NULL) return 1;

    // Esperar bus libre
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

    // Write register address
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C1, addr << 1, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    I2C_SendData(I2C1, reg);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    // Repeated START
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C1, addr << 1, I2C_Direction_Receiver);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    // Leer MSB
    I2C_AcknowledgeConfig(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
    msb = I2C_ReceiveData(I2C1);

    // Leer LSB (último byte -> NACK + STOP)
    I2C_AcknowledgeConfig(I2C1, DISABLE);
    I2C_GenerateSTOP(I2C1, ENABLE);

    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
    lsb = I2C_ReceiveData(I2C1);

    I2C_AcknowledgeConfig(I2C1, ENABLE);

    *value = (msb << 8) | lsb;

    return 0;
}


// I2C Init
void I2C1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    I2C_InitTypeDef I2C_InitStructure;

    // Relojes
    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOC, ENABLE);
    RCC_PB1PeriphClockCmd(RCC_PB1Periph_I2C1, ENABLE);

    // Remapeo I2C1 -> PC1=SCL, PC2=SDA
    GPIO_PinRemapConfig(GPIO_PartialRemap1_I2C1, ENABLE);

    // Configurar pines
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // Liberar bus
    GPIO_SetBits(GPIOC, GPIO_Pin_1 | GPIO_Pin_2);

    // Reset periférico
    I2C_DeInit(I2C1);

    // Configuración I2C
    I2C_InitStructure.I2C_ClockSpeed = 100000;
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress =
        I2C_AcknowledgedAddress_7bit;

    I2C_Init(I2C1, &I2C_InitStructure);

    I2C_Cmd(I2C1, ENABLE);

    printf("I2C init OK\r\n");
}


// SPI init
uint8_t spi_bus_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

    // 1. Relojes
    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOA, ENABLE);
    RCC_PB2PeriphClockCmd(RCC_PB2Periph_SPI1, ENABLE);
    
    // 2. SCK + MOSI
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 3. MISO
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 4. SPI config
    SPI_I2S_DeInit(SPI1);

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;

    SPI_Init(SPI1, &SPI_InitStructure);

    SPI_Cmd(SPI1, ENABLE);

    return 0;
}