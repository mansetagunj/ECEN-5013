/**
 * @brief Test for the APDS9301 sensor
 * 
 * @file apds9301_testmain.c
 * @author Gunj Manseta
 * @date 2018-03-14
 */

#include "my_i2c.h"
#include "apds9301_sensor.h"
#include <unistd.h>

int main()
{
    I2C_MASTER_HANDLE_T i2c;
    int ret = 0;
    if(ret = I2Cmaster_Init(&i2c) !=0)
    {
        printErrorCode(ret);
        printf("[ERROR] I2C Master init failed\n"); 
    }

    ret = APDS9301_poweron();
    if(ret == 0) printf("Sensor ON\n");
    uint8_t sensor_id = 0x50;
    uint8_t data = 0;
    ret  = I2Cmaster_read_byte(APDS9301_SLAVE_ADDR, APDS9301_CTRL_REG, &data);
    if(ret == 0) printf("CTRL REG: %x\n",data);

    ret = APDS9301_readID(&data);
    if(ret == 0) printf("expected: %x ID: %x\n",sensor_id, data);

    while(1)
{
    double lux = APDS9301_getLux();
    if(lux < 0) printf("Error. Lux is negative\n");
    else    printf("Lux: %f\n",lux);
	sleep(2);
}
    if(ret = I2Cmaster_Destroy(&i2c) !=0)
    {
        printErrorCode(ret);
        printf("[ERROR] I2C Master destroy failed\n"); 
    }

}
