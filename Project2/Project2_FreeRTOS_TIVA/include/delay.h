/*
 * delay.h
 *
 *  Created on: 22-Apr-2018
 *      Author: Gunj Manseta
 */

#ifndef DELAY_H_
#define DELAY_H_

#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"

extern uint32_t g_sysClock;

#ifdef DEBUG
#ifndef DEBUG_ERROR
#define DEBUG_ERROR(x) if( ( x ) == 1 ) { taskDISABLE_INTERRUPTS(); while(1); }
#endif
#else
#define configASSERT(x)
#endif

static inline void DelayMs(uint32_t ms)
{
    MAP_SysCtlDelay((g_sysClock/ (1000 * 3))*ms);
}

static inline void DelayUs(uint32_t us)
{
    MAP_SysCtlDelay((g_sysClock/ (1000000 * 3))*us);
}


#endif /* DELAY_H_ */
