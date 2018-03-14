/**
 * @brief Test for the APDS9301 sensor
 * 
 * @file apds9301_testmain.c
 * @author Gunj Manseta
 * @date 2018-03-14
 */

#include "my_i2c.h"
#include "apds9301_sensor.h"
#include "error_data.h"


int main()
{
    I2C_MASTER_HANDLE_T i2c;
    int ret = 0;
    if(ret = I2Cmaster_Init(&i2c) !=0)
    {
        printErrorCode(ret);
        LOG_STDOUT(ERROR "I2C Master init failed\n"); 
    }

    uint8_t sensor_id = 0x50;
    uint8_t data = 0;
    ret = APDS9301_readID(&data);
    printf("expected: %x ID: %x\n",sensor_id, data);

}