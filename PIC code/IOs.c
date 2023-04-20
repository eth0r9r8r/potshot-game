/* 
 * File:   IOs.c
 * Author: ENCM 511 Group 20
 *
 * Created on October 12, 2022, 4:28 PM
 */

#include "xc.h"
#include "TimeDelay.h"
#include "macros.h"
#include <p24fxxxx.h>
#include <p24F16KA101.h>

// Global variables
int CNflag = 0;
int last_in = -1;
int in_reps = -1;
int valid = 0;

// Initializes IO
void IOinit(void)
{
    AD1PCFG = 0xFFFF;  // Turn all analog pins to digital
    
    TRISAbits.TRISA4  = 1;  // Set RA4(CN0) to input
    CNPU1bits.CN0PUE  = 1;  // Enable pull-up resistor on CN0
    CNEN1bits.CN0IE   = 1;  // Enable interrupts on CN0
    
    TRISAbits.TRISA2  = 1;  // Set RA2(CN30) to input
    CNPU2bits.CN30PUE = 1;  // Enable pull-up resistor on CN30
    CNEN2bits.CN30IE  = 1;  // Enable interrupts on CN30
    
    TRISBbits.TRISB4  = 1;  // Set RB4(CN1) to input
    CNPU1bits.CN1PUE  = 1;  // Enable pull-up resistor on CN1
    CNEN1bits.CN1IE   = 1;  // Enable interrupts on CN1
    
    TRISBbits.TRISB8  = 0;  // Set RB8 to output;
    TRISBbits.TRISB9  = 0;  // Set RB9 to output;
    TRISBbits.TRISB7  = 0;  // Set RB7 to output;
    
    IPC4bits.CNIP = 5;  // Set CN interrupt priority to 5
    IFS1bits.CNIF = 0;  // Clear CN interrupt flag
    IEC1bits.CNIE = 1;  // Enable CN interrupts
}

// Idle until input is detected
void waitIO(void)
{
    IFS1bits.CNIF = 0;  // Clear CN interrupt flag
    IEC1bits.CNIE = 1;  // Enable CN interrupts
    
    Idle();  // wait for input
    
    IEC1bits.CNIE = 0; // Disable CN interrupts 
}

// Idle until player releases buttons
void releasePBs(int enterIdle)
{
    IFS1bits.CNIF = 0;  // Clear CN interrupt flag
    IEC1bits.CNIE = 1;  // Enable CN interrupts
    
    if(enterIdle) Idle(); // start in Idle if enterIdle == 1
    
    while(1){
        // break when buttons are released
        if(PORTAbits.RA2 && PORTAbits.RA4 && PORTBbits.RB4) break;
        Idle(); // wait for input change
        delay_ms(100); // debounce
    }
    IEC1bits.CNIE = 0; // Disable CN interrupts
}

/// Checks status of inputs, returns input state after validation.
/// Inputs are valid if they have been held for a full cycle, and if all
/// buttons where released since the last time a pressed state was returned.
int8_t IOcheck(void)
{
    // save state of inputs when function was called
    int pb1 = 1 - PORTAbits.RA2;
    int pb2 = 1 - PORTAbits.RA4;
    int pb3 = 1 - PORTBbits.RB4;
    
    int r; // return value
    
    // encode button combination
    if(pb1 && pb2 && pb3) r = 6;
    else if(pb1 && pb2) r = 3;
    else if(pb2 && pb3) r = 4;
    else if(pb1 && pb3) r = 5;
    else if(pb1) r = 0;
    else if(pb2) r = 1;
    else if(pb3) r = 2;
    else{
        r = -1;
        valid = 1; // the next input is validated  
    }
    
    if(!valid) return -1; // buttons must all be released between inputs
    
    if(r == last_in){
        in_reps++;
        if(in_reps == 1){ // if input has been held for a full cycle:
            valid = 0; // invalidate next input
            return r; // return input code 
        }
        return -1; // input not returned if not held for long enough
    }
    in_reps = 0; // input has not been held for full cycle yet 
    last_in = r; // remember value of last input read
    return -1; // input is not valid yet
}

//ISR
void __attribute__((interrupt, no_auto_psv))_CNInterrupt(void)
{
    CNflag = 1;
    T2CONbits.TON = 0;
    IFS1bits.CNIF = 0;
}