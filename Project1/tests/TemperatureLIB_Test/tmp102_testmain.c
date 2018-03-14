/**
 * @brief Test for tmp102 sensor
 * 
 * @file tmp102_testmain.c
 * @author Gunj Manseta
 * @date 2018-03-14
 */

#include "my_i2c.h"
#include "tmp102_sensor.h"


int main()
{
    I2C_MASTER_HANDLE_T i2c;
    int ret = 0;
    if(ret = I2Cmaster_Init(&i2c) !=0)
    {
        printErrorCode(ret);
        printf("[ERROR] I2C Master init failed\n"); 
    }
    
    double temperature = 0.0;
    uint8_t data[2] = 0;
    ret  = I2Cmaster_read_bytes(TMP102_SLAVE_ADDR, TMP102_REG_CONFIGURATION, data, sizeof(uint16_t));
    if(ret == 0) printf("CONFIG REG: %x\n",*((uint16_t*)data));

    ret = TMP102_getTemp(&temperature, CELCIUS);
    if(ret == 0) printf("C Temp: %f\n",temperature);

    ret = TMP102_getTemp(&temperature, FAHREN);
    if(ret == 0) printf("F Temp: %f\n",temperature);

        ret = TMP102_getTemp(&temperature, KELVIN);
    if(ret == 0) printf("K Temp: %f\n",temperature);

    if(ret = I2Cmaster_Destroy(&i2c) !=0)
    {
        printErrorCode(ret);
        printf("[ERROR] I2C Master destroy failed\n"); 
    }

}