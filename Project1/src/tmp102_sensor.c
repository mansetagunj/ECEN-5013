/**
 * @brief 
 * 
 * @param temp 
 * @param unit 
 * @return int 
 */

#include "my_i2c.h"
#include "tmp102_sensor.h"


int TMP102_getTemp(double *temp, TEMPERATURE_UNIT_T unit)
{
    uint8_t buff[2] = {0};
    int ret = I2Cmaster_read_bytes(TMP102_SLAVE_ADDR, TMP102_REG_TEMPERATURE, buff, sizeof(buff));
    uint16_t temp_raw = (buff[0] << 4) | (buff[1] >> 4);
    if(temp_raw & 0x800)
    {
        temp_raw = (~temp_raw) + 1; 
        *temp = (-1) * (double)temp_raw * 0.0625;
    }
    else
    {
        *temp = ((double)temp_raw)*0.0625;
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