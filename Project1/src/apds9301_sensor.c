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
#include <string.h>

#define APDS9301_SENSOR_ID (0x50)

int APDS9301_test()
{
    uint8_t data;
    int ret = APDS9301_readID(&data);
    if(ret == 0) 
    {
        (APDS9301_SENSOR_ID == (data & 0xF0)) ? 0 : (ret = -1);
    }

    return ret; 
    
}

int APDS9301_write_ThLow(uint16_t thlow)
{
    int ret = I2Cmaster_write_word(APDS9301_SLAVE_ADDR, APDS9301_INT_TH_LL_REG | APDS9301_CMD_WORD_EN, thlow, 0);
    return ret;
}

int APDS9301_write_ThHigh(uint16_t thhigh)
{
    int ret = I2Cmaster_write_word(APDS9301_SLAVE_ADDR, APDS9301_INT_TH_HL_REG | APDS9301_CMD_WORD_EN , thhigh, 0);
    return ret;
}

int APDS9301_read_ThLow(uint16_t *thlow)
{
    int ret = I2Cmaster_read_bytes(APDS9301_SLAVE_ADDR, APDS9301_INT_TH_LL_REG, (uint8_t*)thlow, sizeof(thlow));
    return ret;
}

int APDS9301_read_ThHigh(uint16_t *thhigh)
{
    int ret = I2Cmaster_read_bytes(APDS9301_SLAVE_ADDR, APDS9301_INT_TH_HL_REG, (uint8_t*)thhigh, sizeof(thhigh));
    return ret;
}

int APDS9301_readControlReg(uint8_t *ctrl_reg)
{
    int ret = I2Cmaster_read_byte(APDS9301_SLAVE_ADDR, APDS9301_CTRL_REG, ctrl_reg);
    return ret;
}

int APDS9301_mode_highGain()
{
    uint8_t data;
    int ret = I2Cmaster_read_byte(APDS9301_SLAVE_ADDR, APDS9301_TIMING_REG, &data);
    if(ret != 0)
        return ret;

    data |= (uint8_t)APDS9301_TIMING_GAIN;

    ret = I2Cmaster_write_byte(APDS9301_SLAVE_ADDR, APDS9301_TIMING_REG, data);

    return ret;
}
int APDS9301_mode_lowGain_default()
{
    uint8_t data;
    int ret = I2Cmaster_read_byte(APDS9301_SLAVE_ADDR, APDS9301_TIMING_REG, &data);
    if(ret != 0)
        return ret;

    data &= ~(uint8_t)APDS9301_TIMING_GAIN;

    ret = I2Cmaster_write_byte(APDS9301_SLAVE_ADDR, APDS9301_TIMING_REG, data);

    return ret;

}

int APDS9301_mode_interrupt(uint8_t enable)
{
    uint8_t data;
    int ret = I2Cmaster_read_byte(APDS9301_SLAVE_ADDR, APDS9301_INT_CTRL_REG, &data);
    if(ret != 0)
        return ret;

    if(enable)
        data |= (uint8_t)APDS9301_INTCTRL_IEN;
    else
        data &= ~(uint8_t)APDS9301_INTCTRL_IEN;

    ret = I2Cmaster_write_byte(APDS9301_SLAVE_ADDR, APDS9301_TIMING_REG, data);

    return ret;
}
int APDS9301_clearPendingInterrupt()
{
    int ret = I2Cmaster_write(APDS9301_SLAVE_ADDR, APDS9301_CMD_REG | APDS9301_CMD_INT_CLEAR);
    return ret;    
}

int APDS9301_mode_manualcontrol(uint8_t on)
{
    uint8_t data;
    int ret = I2Cmaster_read_byte(APDS9301_SLAVE_ADDR, APDS9301_TIMING_REG, &data);
    if(ret != 0)
        return ret;

    data &= ~(uint8_t)APDS9301_TIMING_MANUAL(1);
    data |= (uint8_t)APDS9301_TIMING_INTEG(on);

    return ret;
}

int APDS9301_mode_integrationTime(uint8_t x)
{
    uint8_t data;
    int ret = I2Cmaster_read_byte(APDS9301_SLAVE_ADDR, APDS9301_TIMING_REG, &data);
    if(ret != 0)
        return ret;
    
    data &= ~(uint8_t)APDS9301_TIMING_INTEG(3);
    data |= (uint8_t)APDS9301_TIMING_INTEG(x);

    return ret;
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

float APDS9301_getLux()
{
    float ratio, lux = -1;
    uint16_t Ch0, Ch1;

    int ret  = APDS9301_readCh0(&Ch0);
    if(ret)
        return lux;

    ret  = APDS9301_readCh1(&Ch1);
    if(ret)
        return lux;

    if(Ch0 != 0)
        ratio = (float)Ch1/(float)Ch0;
    else
        ratio = 0;

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


uint8_t* APDS9301_memDump()
{
    uint8_t *memdump = (uint8_t*)malloc(15*sizeof(uint8_t));
    memset(memdump, 0 , 15);

    for(uint8_t i = 0 ; i < 0x10; i++)
    {
        if(i == 0x7 ||  i == 0x9 || i == 0xB)
            continue;
        
        int ret = I2Cmaster_read_byte(APDS9301_SLAVE_ADDR, i | APDS9301_CMD_REG, memdump+i);
        if(ret != 0 ) continue;
    }

    return memdump;
}