/* 
 * File:   TimeDelay.c
 * Author: ENCM 511 Group 20
 *
 * Created on October 12, 2022, 4:30 PM
 */

#include "xc.h"
#include <p24fxxxx.h>
#include <p24F16KA101.h>
#include "TimeDelay.h"
#include "macros.h"

// Global variables
int TMR2flag = 0;

// Sets a timer for (time_ms) milliseconds, then enters idle mode.
// Returns 1 if idle was interrupted by timer, 0 if interrupted by input change.
int delay_ms(uint16_t time_ms)
{
    TMR2flag = 0;
    
    // Reset TMR2 and calculate PR2
    TMR2 = 0;
    PR2 = 15.625 * time_ms;
    T2CONbits.TON = 1;  // Activate timer 2
    
    Idle(); // Wait for interrupt
    
    // Return 0 if timer was interrupted before finishing, else return 1
    if(TMR2flag == 1){
        TMR2flag = 0;
        return 1;
    }
    return 0;
}

// Initialize Timer 2
void TMR2init(void)
{
    // Configure Timer 2
    T2CONbits.T32   = 0;  // Use timer 2 as 16-bit timer
    T2CONbits.TCKPS = 3;  // Set pre-scaler to 1:256
    T2CONbits.TCS   = 0;  // Use internal clock
    T2CONbits.TGATE = 0;  // Disable gated time accumulation 
    T2CONbits.TSIDL = 0;  // Set to continue in idle mode
    
    // Configure interrupts
    IPC1bits.T2IP = 4;  // Set timer 2 interrupt priority to 4
    IFS0bits.T2IF = 0;  // Clear timer 2 interrupt flag
    IEC0bits.T2IE = 1;  // Enable timer 2 interrupts
}

// Initialize Timer 1
void TMR1init(void)
{
    // Configure Timer 1
    T1CONbits.TCKPS = 3;  // Set pre-scaler to 1:256
    T1CONbits.TCS   = 0;  // Use internal clock
    T1CONbits.TGATE = 0;  // Disable gated time accumulation 
    T1CONbits.TSIDL = 0;  // Set to continue in idle mode
}

// Sets time period for Timer 1
void set_TMR1(uint16_t time_ms)
{
    IFS0bits.T1IF = 0; // clear flag
    
    // set Timer 1:
    TMR1 = 0;
    if(time_ms) PR1 = 15.625 * time_ms; // update PR2 if new time_ms is given
    T1CONbits.TON = 1;
}

// returns true and reset Timer 1 if Timer one has finished 
int period_elapsed(void)
{
    if(!IFS0bits.T1IF) return 0; // return false if Timer 1 has not finished
    // otherwise, reset Timer 1 and return true
    set_TMR1(0);
    return 1;
}

// stop Timer 1
void stop_TMR1(void)
{
    T1CONbits.TON = 0;
    IFS0bits.T1IF = 0;
}

void __attribute__((interrupt, no_auto_psv))_T2Interrupt(void)
{
    IFS0bits.T2IF = 0;
    T2CONbits.TON = 0;
    TMR2flag = 1;
}
