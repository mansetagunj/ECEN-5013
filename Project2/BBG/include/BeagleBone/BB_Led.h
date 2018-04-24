/**
 * @brief 
 * 
 * @file BB_Led.h
 * @author Gunj Manseta
 * @date 2018-03-10
 */

#ifndef BB_LED_H
#define BB_LED_H

typedef enum
{
    LED0,
    LED1,
    LED2,
    LED3
}USER_LED_T;

/**
 * @brief 
 * 
 * @param lednum 
 * @return int 
 */
int BB_LedON(USER_LED_T lednum);

/**
 * @brief 
 * 
 * @param lednum 
 * @return int 
 */
int BB_LedOFF(USER_LED_T lednum);

/**
 * @brief 
 * 
 * @param lednum 
 * @return int 
 */
int BB_LedDefault();

#endif