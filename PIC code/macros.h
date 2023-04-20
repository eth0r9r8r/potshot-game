/* 
 * File:   macros.h
 * Author: ENCM511 Group 20 
 *
 * Created on December 1, 2022, 9:26 PM
 */

#ifndef MACROS_H
#define	MACROS_H

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#define Nop() {__asm__ volatile ("nop");}
#define ClrWdt() {__asm__ volatile ("clrwdt");}
#define Sleep() {__asm__ volatile ("pwrsav #0");}   // set sleep mode
#define Idle() {__asm__ volatile ("pwrsav #1");}
#define dsen() {__asm__ volatile ("BSET DSCON, #15");}
#define Reset() {__asm__ volatile ("RESET");}

#endif	/* MACROS_H */

