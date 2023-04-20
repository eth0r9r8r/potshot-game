/* 
 * File:   TimeDelay.h
 * Author: Ethan Reed
 *
 * Created on October 12, 2022, 4:30 PM
 */

#ifndef TIMEDELAY_H
#define	TIMEDELAY_H

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#include "stdint.h"

int delay_ms(uint16_t);
void TMR2init(void);
void TMR1init(void);
void set_TMR1(uint16_t);
void stop_TMR1(void);
int period_elapsed(void);


#endif	/* TIMEDELAY_H */

