/**
 * @brief 
 * 
 * @file apds9301_sensor.c
 * @author Gunj Manseta
 * @date 2018-03-14
 */

#include <math.h>
#include "apds9301_sensor.h"
#include"my_i2c.h"
#include "error_data.h"

int APDS9301_init()
{
}

int APDS9301_poweron()
{
    int ret = I2Cmaster_write_byte(APDS9301_SLAVE_ADDR, APDS9301_CTRL_REG, APDS9301_CTRL_POWERON);
    return ret;
}
int APDS9301_powerdown()
{
    int ret = I2Cmaster_write_byte(APDS9301_SLAVE_ADDR, APDS9301_CTRL_REG, APDS9301_CTRL_POWEROFF);
    return ret;
}
int APDS9301_readID(uint8_t *id)
{
    int ret = I2Cmaster_read_byte(APDS9301_SLAVE_ADDR, APDS9301_ID_REG, id);
    return ret;
}

int APDS9301_readCh0(uint16_t *ch0_data)
{
    int ret;
    uint8_t ch0_data_L, ch0_data_H;
    ret = I2Cmaster_read_byte(APDS9301_SLAVE_ADDR, APDS9301_CH0_DATALOW, &ch0_data_L);
    if(ret)
        return ret;

    ret = I2Cmaster_read_byte(APDS9301_SLAVE_ADDR, APDS9301_CH0_DATAHIGH, &ch0_data_H);

    if(!ret)
        *ch0_data = (ch0_data_H << 8) | ch0_data_L ;
    
    return ret;
}

int APDS9301_readCh1(uint16_t *ch1_data)
{
    int ret;
    uint8_t ch1_data_L, ch1_data_H;
    ret = I2Cmaster_read_byte(APDS9301_SLAVE_ADDR, APDS9301_CH1_DATALOW, &ch1_data_L);
    if(ret)
        return ret;

    ret = I2Cmaster_read_byte(APDS9301_SLAVE_ADDR, APDS9301_CH1_DATAHIGH, &ch1_data_H);
    
    if(!ret)
        *ch1_data = (ch1_data_H << 8) | ch1_data_L;
    
    return ret;

}

double APDS9301_getLux()
{
    double ratio, lux = -1;
    uint16_t Ch0, Ch1;

    int ret  = APDS9301_readCh0(&Ch0);
    if(ret)
        return lux;

    ret  = APDS9301_readCh1(&Ch1);
    if(ret)
        return lux;

    ratio = (double)Ch1/(double)Ch0;

    //Calculate LUX - calculations are referred from the Sensor datasheet
	if (ratio > 0 && ratio <= 0.50)
	{
		lux = 0.0304*Ch0 - 0.062*Ch0*(pow(ratio, 1.4));
	}
	else if (ratio > 0.50 && ratio <= 0.61)
	{
		lux = 0.0224*Ch0 - 0.031*Ch1;
	}
	else if (ratio > 0.61 && ratio <= 0.80)
	{
		lux = 0.0128*Ch0 - 0.0153*Ch1;
	}
	else if (ratio > 0.80 && ratio <= 1.30)
	{
		lux = 0.00146*Ch0 - 0.00112*Ch1;
	}
	else if (ratio > 1.30)
	{
		lux = 0;
    }

    return lux;
}