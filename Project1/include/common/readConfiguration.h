/**
 * @brief 
 * 
 * @file readConfiguration.h
 * @author Gunj Manseta
 * @date 2018-03-17
 */

#ifndef READCONIFG_H
#define READCONIFG_H

/**
 * @brief 
 * 
 * @return char* 
 */
char* configdata_getLogpath();

/**
 * @brief 
 * 
 * @return uint32_t 
 */
uint32_t configdata_getSetupTime();

/**
 * @brief 
 * 
 * @return uint32_t 
 */
uint32_t configdata_getAliveTimeout();

/**
 * @brief Should be called in the main task at the beginning
 * 
 * @return int 
 */
int configdata_setup();

/**
 * @brief Should be called at teh end of main task. If not called, memory leak will occur
 * 
 */
void configdata_flush();


#endif