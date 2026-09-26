#include "debug.h"
#include "utils.h"
#include "lm75.h"

int main(void)
{
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);

    printf("\r\n========================\r\n");
    printf("   I2C INA3221 LOOP     \r\n");
    printf("========================\r\n");

    I2C1_Init();

    uint8_t ina_addr = 0x40;
    uint16_t manu_id = 0;
    uint16_t device_id = 0;

    while(1)
    {
        printf("\r\n========================\r\n");

        // -------------------------------------------------
        // 1. SCAN I2C BUS
        // -------------------------------------------------
        printf("--- I2C SCAN ---\r\n");

        for(uint8_t addr = 0x08; addr < 0x78; addr++)
        {
            if(i2c_probe(addr))
            {
                printf("Device found at 0x%02X\r\n", addr);
            }

            Delay_Ms(2);
        }

        // -------------------------------------------------
        // 2. READ INA3221 ID
        // -------------------------------------------------
        printf("\r\n--- INA3221 ID ---\r\n");

        if(i2c_read_reg_16(ina_addr, 0xFE, &manu_id) == 0)
        {
            printf("Manufacturer ID: 0x%04X\r\n", manu_id);
        }
        else
        {
            printf("Failed to read Manufacturer ID\r\n");
        }

        if(i2c_read_reg_16(ina_addr, 0xFF, &device_id) == 0)
        {
            printf("Device ID: 0x%04X\r\n", device_id);
        }
        else
        {
            printf("Failed to read Device ID\r\n");
        }


        // -------------------------------------------------
        // 3. TEMP READING
        // -------------------------------------------------
        
        printf("\r\nTEMPERATURE AFTER TEST\r\n");

        lm75_t lm75;

        float value = 0;

        if (lm75_Init(&lm75, 0x48) != 0)
        {
            printf("ERROR inicializando LM75\r\n");
        }
        else
        {
            if (lm75_read_celsius_temp(&lm75, &value) != 0)
            {
                printf("TEMP read ERROR\r\n");
            }
            else
            {
                int temp_x10 = (int)(value * 10.0f);

                printf("TEMP = %d.%d C\r\n",
                    temp_x10 / 10,
                    temp_x10 % 10);
            }
        }


        // -------------------------------------------------
        // 4. DELAY LOOP
        // -------------------------------------------------
        printf("\r\nNext cycle in 3 seconds...\r\n");
        Delay_Ms(3000);
    }
}