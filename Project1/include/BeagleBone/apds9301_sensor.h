/**
 * @brief 
 * 
 * @file apds9301_sensor.h
 * @author Gunj Manseta
 * @date 2018-03-13
 */


#ifndef APDS9301SENSOR_H
#define APDS9301SENSOR_H

#include <stdint.h>

#define APDS9301_SLAVE_ADDR     (0x39)

#define APDS9301_CMD_REG        (0x80)

/* REGISTERS */
#define APDS9301_CTRL_REG       (0x00) | APDS9301_CMD_REG
#define APDS9301_TIMING_REG     (0x01) | APDS9301_CMD_REG
#define APDS9301_ID_REG         (0x0A) | APDS9301_CMD_REG
#define APDS9301_CH0_DATALOW    (0x0C) | APDS9301_CMD_REG
#define APDS9301_CH0_DATAHIGH   (0x0D) | APDS9301_CMD_REG
#define APDS9301_CH1_DATALOW    (0x0E) | APDS9301_CMD_REG
#define APDS9301_CH1_DATAHIGH   (0x0F) | APDS9301_CMD_REG

/* Bit fields in Registers */
#define APDS9301_CTRL_POWERON   (0x03)   
#define APDS9301_CTRL_POWEROFF  (0x00)


int APDS9301_poweron();
int APDS9301_powerdown();
int APDS9301_readID(uint8_t *id);
int APDS9301_readCh0(uint16_t *ch0_data);
int APDS9301_readCh1(uint16_t *ch1_data);

double APDS9301_getLux();


#endif