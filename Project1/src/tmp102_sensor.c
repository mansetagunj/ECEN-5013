/**
 * @brief 
 * 
 * @param temp 
 * @param unit 
 * @return int 
 */

#include <string.h>
#include "my_i2c.h"
#include "tmp102_sensor.h"

const TMP102_CONFIG_REG_SETTINGS_T TMP102_CONFIG_DEFAULT = 
{
    .SD_MODE = 0,
    .TM_MODE = 0,
    .POL     = 0,
    .OS      = 0,
    .EM_MODE = 0,
    .CR      = 2
};

int TMP102_setmode_allDefault()
{
    int ret = TMP102_setMode_CR_4HZ_default();
    if(ret) return ret;
    ret = TMP102_setMode_SD_Continuous_default();
    if(ret) return ret;
    ret = TMP102_setMode_ALERT_ActiveLow_default();
    if(ret) return ret;
    ret = TMP102_setMode_TM_ComparatorMode_default();
    if(ret) return ret;
    ret = TMP102_setMode_EM_NormalMode_default();
    return ret;
}
int TMP102_setMode(TMP102_CONFIG_REG_SETTINGS_T config)
{
    int ret = I2Cmaster_write_word(TMP102_SLAVE_ADDR, TMP102_REG_CONFIGURATION,*((uint16_t*)&config), 0);

    return ret;
}

int TMP102_setMode_SD_PowerSaving()
{
    uint16_t config_data = 0;

     /* Reading the already configured values in the sensor */
    int ret = I2Cmaster_read_bytes(TMP102_SLAVE_ADDR, TMP102_REG_CONFIGURATION,(uint8_t*)&config_data, sizeof(config_data));
    if(ret == -1)
        return ret;

    config_data |= (uint16_t)TMP102_CONFIG_SD; 

    ret = I2Cmaster_write_word(TMP102_SLAVE_ADDR, TMP102_REG_CONFIGURATION, config_data, 0);

    return ret;

}
int TMP102_setMode_SD_Continuous_default()
{
    uint16_t config_data = 0;

     /* Reading the already configured values in the sensor */
    int ret = I2Cmaster_read_bytes(TMP102_SLAVE_ADDR, TMP102_REG_CONFIGURATION,(uint8_t*)&config_data, sizeof(config_data));
    if(ret == -1)
        return ret;

    config_data &= ~((uint16_t)TMP102_CONFIG_SD); 

    ret = I2Cmaster_write_word(TMP102_SLAVE_ADDR, TMP102_REG_CONFIGURATION, config_data, 0);

    return ret;
}

int TMP102_setMode_TM_ComparatorMode_default()
{
    uint16_t config_data = 0;

     /* Reading the already configured values in the sensor */
    int ret = I2Cmaster_read_bytes(TMP102_SLAVE_ADDR, TMP102_REG_CONFIGURATION,(uint8_t*)&config_data, sizeof(config_data));
    if(ret == -1)
        return ret;

    config_data &= ~((uint16_t)TMP102_CONFIG_TM); 

    ret = I2Cmaster_write_word(TMP102_SLAVE_ADDR, TMP102_REG_CONFIGURATION, config_data, 0);

    return ret;

}
int TMP102_setMode_TM_InterruptMode()
{
    uint16_t config_data = 0;

     /* Reading the already configured values in the sensor */
    int ret = I2Cmaster_read_bytes(TMP102_SLAVE_ADDR, TMP102_REG_CONFIGURATION,(uint8_t*)&config_data, sizeof(config_data));
    if(ret == -1)
        return ret;

    config_data |= (uint16_t)TMP102_CONFIG_TM; 

    ret = I2Cmaster_write_word(TMP102_SLAVE_ADDR, TMP102_REG_CONFIGURATION, config_data, 0);

    return ret;

}
int TMP102_setMode_ALERT_ActiveLow_default()
{
    uint16_t config_data = 0;

     /* Reading the already configured values in the sensor */
    int ret = I2Cmaster_read_bytes(TMP102_SLAVE_ADDR, TMP102_REG_CONFIGURATION,(uint8_t*)&config_data, sizeof(config_data));
    if(ret == -1)
        return ret;

    config_data &= ~((uint16_t)TMP102_CONFIG_POL); 

    ret = I2Cmaster_write_word(TMP102_SLAVE_ADDR, TMP102_REG_CONFIGURATION, config_data, 0);

    return ret;

}
int TMP102_setMode_ALERT_ActiveHigh()
{
    uint16_t config_data = 1;

     /* Reading the already configured values in the sensor */
    int ret = I2Cmaster_read_bytes(TMP102_SLAVE_ADDR, TMP102_REG_CONFIGURATION,(uint8_t*)&config_data, sizeof(config_data));
    if(ret == -1)
        return ret;

    config_data |= (uint16_t)TMP102_CONFIG_POL; 

    ret = I2Cmaster_write_word(TMP102_SLAVE_ADDR, TMP102_REG_CONFIGURATION, config_data, 0);

    return ret;

}
int TMP102_setMode_EM_NormalMode_default()
{
    uint16_t config_data = 0;

     /* Reading the already configured values in the sensor */
    int ret = I2Cmaster_read_bytes(TMP102_SLAVE_ADDR, TMP102_REG_CONFIGURATION,(uint8_t*)&config_data, sizeof(config_data));
    if(ret == -1)
        return ret;

    config_data &= ~((uint16_t)TMP102_CONFIG_EM); 

    ret = I2Cmaster_write_word(TMP102_SLAVE_ADDR, TMP102_REG_CONFIGURATION, config_data, 0);

    return ret;


}
int TMP102_setMode_EM_ExtendedMode()
{
    uint16_t config_data = 0;

     /* Reading the already configured values in the sensor */
    int ret = I2Cmaster_read_bytes(TMP102_SLAVE_ADDR, TMP102_REG_CONFIGURATION,(uint8_t*)&config_data, sizeof(config_data));
    if(ret == -1)
        return ret;

    config_data |= (uint16_t)TMP102_CONFIG_EM; 

    ret = I2Cmaster_write_word(TMP102_SLAVE_ADDR, TMP102_REG_CONFIGURATION, config_data, 0);

    return ret;

}
int TMP102_setMode_CR_250mHZ()
{
    uint16_t config_data = 0;

     /* Reading the already configured values in the sensor */
    int ret = I2Cmaster_read_bytes(TMP102_SLAVE_ADDR, TMP102_REG_CONFIGURATION,(uint8_t*)&config_data, sizeof(config_data));
    if(ret == -1)
        return ret;

    config_data &= ~((uint16_t)TMP102_CONFIG_CR(3)); 

    ret = I2Cmaster_write_word(TMP102_SLAVE_ADDR, TMP102_REG_CONFIGURATION, config_data, 0);

    return ret;

}
int TMP102_setMode_CR_1HZ()
{
    uint16_t config_data = 0;

     /* Reading the already configured values in the sensor */
    int ret = I2Cmaster_read_bytes(TMP102_SLAVE_ADDR, TMP102_REG_CONFIGURATION,(uint8_t*)&config_data, sizeof(config_data));
    if(ret == -1)
        return ret;

    config_data &= ~((uint16_t)TMP102_CONFIG_CR(3));
    config_data |= ((uint16_t)TMP102_CONFIG_CR(1)); 

    ret = I2Cmaster_write_word(TMP102_SLAVE_ADDR, TMP102_REG_CONFIGURATION, config_data, 0);

    return ret;

}
int TMP102_setMode_CR_4HZ_default()
{
    uint16_t config_data = 0;

     /* Reading the already configured values in the sensor */
    int ret = I2Cmaster_read_bytes(TMP102_SLAVE_ADDR, TMP102_REG_CONFIGURATION,(uint8_t*)&config_data, sizeof(config_data));
    if(ret == -1)
        return ret;

    config_data &= ~((uint16_t)TMP102_CONFIG_CR(3));
    config_data |= ((uint16_t)TMP102_CONFIG_CR(2)); 

    ret = I2Cmaster_write_word(TMP102_SLAVE_ADDR, TMP102_REG_CONFIGURATION, config_data, 0);

    return ret;

}

int TMP102_setMode_CR_8HZ()
{
    uint16_t config_data = 0;

     /* Reading the already configured values in the sensor */
    int ret = I2Cmaster_read_bytes(TMP102_SLAVE_ADDR, TMP102_REG_CONFIGURATION,(uint8_t*)&config_data, sizeof(config_data));
    if(ret == -1)
        return ret;

    config_data |= ((uint16_t)TMP102_CONFIG_CR(3)); 

    ret = I2Cmaster_write_word(TMP102_SLAVE_ADDR, TMP102_REG_CONFIGURATION, config_data, 0);

    return ret;

}
int TMP102_readMode_ALERT(uint8_t *al_bit)
{
    uint16_t config_data = 0;

     /* Reading the already configured values in the sensor */
    int ret = I2Cmaster_read_bytes(TMP102_SLAVE_ADDR, TMP102_REG_CONFIGURATION,(uint8_t*)&config_data, sizeof(config_data));
    if(ret == -1)
        return ret;

    *al_bit = (config_data & ((uint16_t)TMP102_CONFIG_AL))>>13; 

    return ret;
}

int TMP102_write_Tlow(float tlow_C)
{
    if(tlow_C < -56.0f || tlow_C > 151.0f)
        tlow_C = 75.0f;
    
    tlow_C /= 0.0625;
    uint16_t tl;

    if(tlow_C > 0)
    {
        tl = ((uint16_t)tlow_C << 4);
        tl &= 0x7FFF;
    }
    else
    {
        tlow_C = -1 * tlow_C;
        tl = (uint16_t)tlow_C;
        tl = ~(tl) + 1;
        tl = tl << 4; 
    }  
    
    int ret = I2Cmaster_write_word(TMP102_SLAVE_ADDR, TMP102_REG_TLOW, tl, 0);
    if(ret == -1)
        return ret;

    #ifdef TEST_I2C
    uint16_t retTlow =0;
    ret = I2Cmaster_read_bytes(TMP102_SLAVE_ADDR, TMP102_REG_TLOW,(uint8_t*)&retTlow, sizeof(retTlow));
    if(ret == -1)
        return ret;
    
    assert(retTlow == tl);
    assert_int_equal(retTlow,tl);
    #endif
         
}

int TMP102_write_Thigh(float thigh_C)
{

    if(thigh_C < -56.0f || thigh_C > 151.0f)
        thigh_C = 80.0f;
    
    thigh_C /= 0.0625;
    uint16_t th;

    if(thigh_C > 0)
    {
        th = ((uint16_t)thigh_C << 4);
        th &= 0x7FFF;
    }
    else
    {
        thigh_C = -1 * thigh_C;
        th = (uint16_t)thigh_C;
        th = ~(th) + 1;
        th = th << 4; 
    }  
    
    int ret = I2Cmaster_write_word(TMP102_SLAVE_ADDR, TMP102_REG_TLOW, th, 0);
    if(ret == -1)
        return ret;

    #ifdef TEST_I2C
    uint16_t ret =0;
    ret = I2Cmaster_read_bytes(TMP102_SLAVE_ADDR, TMP102_REG_TLOW,(uint8_t*)&ret, sizeof(ret));
    if(ret == -1)
        return ret;
    
    assert(ret == th);
    assert_true(ret == th);
    #endif


}

int TMP102_read_Tlow(float *tlow_C)
{
    uint16_t tlow =0;

    int ret = I2Cmaster_read_bytes(TMP102_SLAVE_ADDR, TMP102_REG_TLOW,(uint8_t*)&tlow, sizeof(ret));
    if(ret == -1)
        return ret;

    if(tlow & 0x800)
    {
        tlow = (~tlow) + 1; 
        *tlow_C = (-1) * (float)tlow * 0.0625;
    }
    else
    {
        *tlow_C = ((float)tlow)*0.0625;
    }

    return ret;

}

int TMP102_read_Thigh(float *thigh_C)
{
    uint16_t thigh =0;

    int ret = I2Cmaster_read_bytes(TMP102_SLAVE_ADDR, TMP102_REG_TLOW,(uint8_t*)&thigh, sizeof(ret));
    if(ret == -1)
        return ret;

    if(thigh & 0x800)
    {
        thigh = (~thigh) + 1; 
        *thigh_C = (-1) * (float)thigh * 0.0625;
    }
    else
    {
        *thigh_C = ((float)thigh)*0.0625;
    }

    return ret;

}


int TMP102_getTemp(float *temp, TEMPERATURE_UNIT_T unit)
{
    uint8_t buff[2] = {0};
    int ret = I2Cmaster_read_bytes(TMP102_SLAVE_ADDR, TMP102_REG_TEMPERATURE, buff, sizeof(buff));
    if(ret == -1)
        return ret;

    /* We get MSB(15:8) in buff[0] and LSB(7:4) in buff[1] */
    uint16_t temp_raw = (buff[0] << 4) | (buff[1] >> 4);
    if(temp_raw & 0x800)
    {
        temp_raw = (~temp_raw) + 1; 
        *temp = (-1) * (float)temp_raw * 0.0625;
    }
    else
    {
        *temp = ((float)temp_raw)*0.0625;
    }

    if(unit == FAHREN)
    {
        *temp = (*temp * 1.8) + 32;
    }
    else if(unit == KELVIN)
    {
        *temp += 273.15;
    }

    return ret;
}

uint16_t* TMP102_memDump()
{
    uint16_t *memdump = (uint16_t*)malloc(4*sizeof(uint16_t));
    if(NULL == memdump)
        return NULL;
        
    memset(memdump, 0 , 4);

    for(uint8_t i = 0 ; i < 0x4; i++)
    {      
        int ret = I2Cmaster_read_bytes(TMP102_SLAVE_ADDR, i , (uint8_t*)(memdump+i), sizeof(uint16_t));
        memdump[i] = (memdump[i]<<8) | (memdump[i]>>8);
        
        //float temp = (float)(memdump[i]>>4) * 0.0625;
        //printf(" - F:%.02f - ",temp);
    }

    return memdump;
}