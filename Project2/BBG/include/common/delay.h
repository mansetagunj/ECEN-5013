/*
 * delay.h
 *
 *  Created on: 22-Apr-2018
 *      Author: Gunj Manseta
 */

#ifndef DELAY_H_
#define DELAY_H_

#include <stdint.h>
#include <unistd.h>

/** These functions does not guarentee the sleep time mentioned */

static inline void DelayS(uint32_t s)
{
    sleep(s);
}
static inline void DelayMs(uint32_t ms)
{
    usleep(ms*1000);
}

static inline void DelayUs(uint32_t us)
{
    usleep(us);
}


#endif /* DELAY_H_ */
