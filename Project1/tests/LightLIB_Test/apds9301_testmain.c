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
    ret  = APDS9301_readControlReg(&data);
    if(ret == 0) printf("CTRL REG: %x\n",data);

    uint16_t tlow;
    ret = APDS9301_read_ThLow(&tlow);
    if(ret == 0) printf("READ TLOW 0x%x\n",tlow);

    tlow = 0xBB11;
    ret = APDS9301_write_ThLow(tlow);
    if(ret == 0) printf("WRITE TLOW 0x%x\n",tlow);

    tlow = 0xaaaa;
    ret = APDS9301_read_ThLow(&tlow);
    if(ret == 0) printf("READ TLOW 0x%x\n",tlow);
    
    ret = APDS9301_mode_highGain();
    if(ret != 0) printf("ERROR\n");

    uint8_t *memdump = APDS9301_memDump();
    printf("----SENSOR DUMP-----\n");
    for(uint8_t i = 0; i < 15; i++)
        printf("%02dh : 0x%x\n",i,memdump[i]);
    free(memdump);
    printf("---------\n");

    ret = APDS9301_mode_lowGain_default();
    if(ret != 0) printf("ERROR\n");

    ret = APDS9301_readID(&data);
    if(ret == 0) printf("expected: %x ID: %x\n",sensor_id, data);

    while(1)
    {
        float lux = APDS9301_getLux();
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
