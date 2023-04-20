/* 
 * File:   UART2.h
 * Author: rvyas
 *
 * 
 */

#ifndef UART2_H
#define	UART2_H

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

void InitUART2(void);
void XmitUART2(char, unsigned int);

void __attribute__ ((interrupt, no_auto_psv)) _U2TXInterrupt(void); 
void __attribute__ ((interrupt, no_auto_psv)) _U2RXInterrupt(void);

void Disp2Hex(unsigned int);
void Disp2Hex32(unsigned long int);
unsigned int Disp2String(char*);
void Disp2Dec(unsigned int);
void clearTerm(unsigned int);
int readByte(void);

#endif	/* UART2_H */
