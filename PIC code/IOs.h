/* 
 * File:   IOs.h
 * Author: ENCM 511 Group 20
 *
 * Created on October 12, 2022, 4:28 PM
 */

#ifndef IOS_H
#define	IOS_H

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

void IOinit(void);
int8_t IOcheck(void);;
void waitIO(void);
void releasePBs(int);

#endif	/* IOS_H */

