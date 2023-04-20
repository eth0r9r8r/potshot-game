/*
 * File:   ADC.c
 * Author: ENCM 511 Group 20
 *
 * Created on November 14, 2022, 3:48 PM
 */


#include "xc.h"
#include "UART2.h"
#include <p24fxxxx.h>
#include <p24F16KA101.h>
#include "macros.h"

void ADCinit(void)
{
    AD1CON1bits.ADON = 1; //Turn on ADC
    
    AD1CON1bits.ADSIDL = 0; //continue in idle mode
    AD1CON1bits.FORM = 0; //Sets output as integer
    AD1CON1bits.SSRC = 0b111; //Internal counter controls conversion
    AD1CON1bits.ASAM = 0; //Manual sampling start
    
    AD1CON2bits.VCFG = 0; //USe AVdd and AVss as reference
    AD1CON2bits.CSCNA = 0;// Do not scan inputs
    AD1CON2bits.BUFM = 0; //Use 16 word buffer
    AD1CON2bits.ALTS = 0; //Always use MUX A
    AD1CON2bits.SMPI = 0; //Interrupts after every conversion
    
    AD1CON3bits.ADRC = 0; //Use system clock
    AD1CON3bits.SAMC = 0b11111; //Use slowest sampling
    
    AD1CHSbits.CH0NA = 0; //Channel 0 negative input is VR-
    AD1CHSbits.CH0SA = 0b0101; //Positive input is AN5
    
    AD1PCFGbits.PCFG5 = 0; //Configure AN5 as analog only
    
    IPC3bits.AD1IP = 6; //Set ADC interrupt priority
}

uint16_t do_ADC(void)
{
    uint16_t SampleValue;
    
    AD1CON1bits.ADON = 1; //Turn on ADC
    
    IFS0bits.AD1IF = 0; //Clear interrupt flag
    IEC0bits.AD1IE = 1; //Enable ADC interrupts
    
    AD1CON1bits.SAMP = 1; //Start sampling
    
    while(AD1CON1bits.DONE == 0)
    {
       Idle(); //Will be woken by ADC interrupt
    }
    
    IEC0bits.AD1IE = 0; //Disable ADC interrupts
    
    SampleValue = ADC1BUF0 - 1; //Read value from buffer
    AD1CON1bits.SAMP = 0;   //Stop sampling
    AD1CON1bits.ADON = 0;   //Turn off ADC
    
    return(SampleValue); //return sampled voltage
}

void __attribute__((interrupt, no_auto_psv)) _ADC1Interrupt(void)
{
    IFS0bits.AD1IF = 0; //Clear interrupt flag
}