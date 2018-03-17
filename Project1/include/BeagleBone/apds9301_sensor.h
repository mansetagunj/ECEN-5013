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
#define APDS9301_CMD_WORD_EN    (1<<5)
#define APDS9301_CMD_INT_CLEAR  (1<6)

/* REGISTERS */
#define APDS9301_CTRL_REG           (0x00) | APDS9301_CMD_REG
#define APDS9301_TIMING_REG         (0x01) | APDS9301_CMD_REG
#define APDS9301_ID_REG             (0x0A) | APDS9301_CMD_REG
#define APDS9301_INT_CTRL_REG       (0x06) | APDS9301_CMD_REG
#define APDS9301_CH0_DATALOW        (0x0C) | APDS9301_CMD_REG
#define APDS9301_CH0_DATAHIGH       (0x0D) | APDS9301_CMD_REG
#define APDS9301_CH1_DATALOW        (0x0E) | APDS9301_CMD_REG
#define APDS9301_CH1_DATAHIGH       (0x0F) | APDS9301_CMD_REG
#define APDS9301_INT_TH_LL_REG      (0x02) | APDS9301_CMD_REG 
#define APDS9301_INT_TH_LH_REG      (0x03) | APDS9301_CMD_REG
#define APDS9301_INT_TH_HL_REG      (0x04) | APDS9301_CMD_REG
#define APDS9301_INT_TH_HH_REG      (0x05) | APDS9301_CMD_REG


/* Bit fields in Registers */
#define APDS9301_CTRL_POWERON       (0x03)   
#define APDS9301_CTRL_POWEROFF      (0x00)
#define APDS9301_INTCTRL_IEN        (1<<4)   
#define APDS9301_TIMING_GAIN        (1<<4)
#define APDS9301_TIMING_INTEG(x)    (x)
#define APDS9301_TIMING_MANUAL(x)   (x<<3)


#define  APDS9301_mode_interruptEnable()            APDS9301_mode_interrupt(1)
#define  APDS9301_mode_interruptDisable_default()   APDS9301_mode_interrupt(0)

#define APDS9301_mode_integrationTime0()            APDS9301_mode_integrationTime(0)    
#define APDS9301_mode_integrationTime1()            APDS9301_mode_integrationTime(1)
#define APDS9301_mode_integrationTime2_default()    APDS9301_mode_integrationTime(2)
#define APDS9301_mode_integrationTime3()            APDS9301_mode_integrationTime(3)

#define APDS9301_mode_manualcontrolON()             APDS9301_mode_manualcontrol(1)
#define APDS9301_mode_manualcontrolOFF_default()    APDS9301_mode_manualcontrol(0)


/**
 * @brief Gives a memdump of 15 len. 
 *  **IMP** must free the address using return pointer 
 * @return uint8_t* 
 */
uint8_t* APDS9301_memDump();

/**
 * @brief 
 * 
 * @param thlow 
 * @return int 
 */
int APDS9301_write_ThLow(uint16_t thlow);

/**
 * @brief 
 * 
 * @param thhigh 
 * @return int 
 */
int APDS9301_write_ThHigh(uint16_t thhigh);

/**
 * @brief 
 * 
 * @param thlow 
 * @return int 
 */
int APDS9301_read_ThLow(uint16_t *thlow);

/**
 * @brief 
 * 
 * @param thhigh 
 * @return int 
 */
int APDS9301_read_ThHigh(uint16_t *thhigh);

/**
 * @brief 
 * 
 * @param ctrl_reg 
 * @return int 
 */
int APDS9301_readControlReg(uint8_t *ctrl_reg);

/**
 * @brief 
 * 
 * @return int 
 */
int APDS9301_mode_highGain();

/**
 * @brief 
 * 
 * @return int 
 */
int APDS9301_mode_lowGain_default();

/**
 * @brief 
 * 
 * @param on 
 * @return int 
 */
int APDS9301_mode_manualcontrol(uint8_t on);

/**
 * @brief 
 * 
 * @param x 
 * @return int 
 */
int APDS9301_mode_integrationTime(uint8_t x);

/**
 * @brief 
 * 
 * @param enable 
 * @return int 
 */
int APDS9301_mode_interrupt(uint8_t enable);

/**
 * @brief 
 * 
 * @return int 
 */
int APDS9301_clearPendingInterrupt();


/**
 * @brief 
 * 
 * @return int 
 */
int APDS9301_poweron();

/**
 * @brief 
 * 
 * @return int 
 */
int APDS9301_powerdown();

/**
 * @brief 
 * 
 * @param id 
 * @return int 
 */
int APDS9301_readID(uint8_t *id);

/**
 * @brief 
 * 
 * @param ch0_data 
 * @return int 
 */
int APDS9301_readCh0(uint16_t *ch0_data);

/**
 * @brief 
 * 
 * @param ch1_data 
 * @return int 
 */
int APDS9301_readCh1(uint16_t *ch1_data);

/**
 * @brief 
 * 
 * @return float 
 */
float APDS9301_getLux();

/**
 * @brief 
 * 
 * @return int 
 */
int APDS9301_test();


#endif