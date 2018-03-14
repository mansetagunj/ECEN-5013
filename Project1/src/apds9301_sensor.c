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

int APDS9301_readCh0(uint16_t *ch0_data);
int APDS9301_readCh1(uint16_t *ch1_data);

double APDS9301_getLux()
{
    double ratio, lux;
    uint16_t Ch0, Ch1;

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