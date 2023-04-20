/* 
 * File:   Interfacing.h
 * Author: ENCM 511 Group 20
 *
 * Created on November 28, 2022, 4:02 PM
 */

#ifndef INTERFACING_H
#define	INTERFACING_H

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#include "stdint.h"

typedef struct {
    int symbol;
    int arrival;
    int position;
} Prompt;

int title(void);
void disp_play(int*, int);
int pause_menu(void);
int load_level(Prompt*);

#endif	/* INTERFACING_H */

