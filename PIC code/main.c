/* 
 * File:   main.c
 * Author: ENCM 511 Group 20
 *
 * Created on October 9, 2022, 7:46 PM
 */

#include "time.h"
#include "Interfacing.h"
#include "stdlib.h"
#include "string.h"
#include "xc.h"
#include "IOs.h"
#include "TimeDelay.h"
#include <p24fxxxx.h>
#include <p24F16KA101.h>
#include "UART2.h"
#include "ADC.h"
#include "macros.h"


//// CONFIGURATION BITS - PRE-PROCESSOR DIRECTIVES ////

// Code protection 
#pragma config BSS = OFF // Boot segment code protect disabled
#pragma config BWRP = OFF // Boot sengment flash write protection off
#pragma config GCP = OFF // general segment code protecion off
#pragma config GWRP = OFF

// CLOCK CONTROL 
#pragma config IESO = OFF    // 2 Speed Startup disabled
#pragma config FNOSC = FRC  // Start up CLK = 8 MHz
#pragma config FCKSM = CSECMD // Clock switching is enabled, clock monitor disabled
#pragma config SOSCSEL = SOSCLP // Secondary oscillator for Low Power Operation
#pragma config POSCFREQ = MS  //Primary Oscillator/External clk freq betwn 100kHz and 8 MHz. Options: LS, MS, HS
#pragma config OSCIOFNC = ON  //CLKO output disabled on pin 8, use as IO. 
#pragma config POSCMOD = NONE  // Primary oscillator mode is disabled

// WDT
#pragma config FWDTEN = OFF // WDT is off
#pragma config WINDIS = OFF // STANDARD WDT/. Applicable if WDT is on
#pragma config FWPSA = PR32 // WDT is selected uses prescaler of 32
#pragma config WDTPS = PS1 // WDT postscler is 1 if WDT selected

//MCLR/RA5 CONTROL
#pragma config MCLRE = OFF // RA5 pin configured as input, MCLR reset on RA5 diabled

//BOR  - FPOR Register
#pragma config BORV = LPBOR // LPBOR value=2V is BOR enabled
#pragma config BOREN = BOR0 // BOR controlled using SBOREN bit
#pragma config PWRTEN = OFF // Powerup timer disabled
#pragma config I2C1SEL = PRI // Default location for SCL1/SDA1 pin

//JTAG FICD Register
#pragma config BKBUG = OFF // Background Debugger functions disabled
#pragma config ICS = PGx2 // PGC2 (pin2) & PGD2 (pin3) are used to connect PICKIT3 debugger

// Deep Sleep RTCC WDT
#pragma config DSWDTEN = OFF // Deep Sleep WDT is disabled
#pragma config DSBOREN = OFF // Deep Sleep BOR is disabled
#pragma config RTCOSC = LPRC// RTCC uses LPRC 32kHz for clock
#pragma config DSWDTOSC = LPRC // DeepSleep WDT uses Lo Power RC clk
#pragma config DSWDTPS = DSWDTPS7 // DSWDT postscaler set to 32768

// GLOBAL VARIABLES
extern int CNflag;
      
int main(void)
{
    // Initialize IO
    IOinit();
    
    // Initialize Timers
    TMR2init();
    TMR1init();
    
    // Initialize UART2
    InitUART2();
    
    // Initialize ADC
    ADCinit();
    
    // Make sure display is on title screen
    XmitUART2(0,1);
    
    // stay on title screen until input is given
    if(!title()) Reset(); // Reset if player quits display program
    
    // set max time and target count
    const int IN_CAP = 63;
    const int TIME_CAP = 120;
    
    Prompt incoming[IN_CAP]; // array of incoming prompts
    
    // keep track of size and status of incoming
    int in_dex = 0;  // index of next prompt to enter play
    int in_fin = 0;  // flag set when all prompts have entered play 
    int in_size = 0;  // number of prompts in incoming
    
    // move to level selection
    in_size = load_level(incoming);
    
    // variables for keeping track of game state
    int cursor;  // new cursor position
    int cursor_x;  //  previous cursor position
    int prompts[16];  // keep track of prompts in play
    for(int i=0; i < 16; i++){ // start with no prompts in play
        prompts[i] = -1;
    }
    int lives[16] = {0};  // remaining time of each prompt
    int score = 0;
    int p_count = 0;  // number of prompts in play
    int input; 
    int time_s = 0;  // time passed in current level (in seconds)
    int i;
    
    // disable CN interrupts and calculate starting cursor position
    IEC1bits.CNIE = 0;
    cursor = do_ADC() / (1023.0 / 60.0);
    
    // start periodic timer
    set_TMR1(1000);
    
    while(1)
    {
        // Tasks to be performed every second:
        if(period_elapsed()){
            time_s++;  // increment time
            // move prompts into play when it is time:
            while(!in_fin && incoming[in_dex].arrival == time_s){
                i = incoming[in_dex].position;  // get position of new prompt
                prompts[i] = incoming[in_dex].symbol; // put prompt in place
                lives[i] = 3; // set prompt's remaining time to 3sec
                p_count++;
                in_dex++; 
                if(in_dex >= in_size) in_fin = 1;
            }
            // display score when game is done:
            if((!p_count && in_fin) || time_s == TIME_CAP){
                XmitUART2(score + 64, 1);
                delay_ms(1000);
                releasePBs(1);
                Reset();
            }
            // remove prompts in play after three seconds:
            for(i = 0; i < 16; i++){
                if (prompts[i] >= 10 || prompts[i] == -1) continue;
                if(!lives[i]){
                    prompts[i] = -1;
                    p_count--;
                }
                else lives[i]--;
            }
        }
        
        input = IOcheck(); // check input
        if(input != -1){ // if there has been a valid input change:
            // pause game if cursor is out of bounds
            if(cursor == 0 || cursor == 19){
                // if player opts to quit:
                if(!pause_menu()){ 
                    releasePBs(0); // wait for player to release buttons
                    Reset(); // return to main menu
                }
                // else:
                XmitUART2(30,1); // return to game
                IOcheck(); // invalidate IO
            }
            // ignore input if cursor is on game boundary
            else if(cursor == 1 || cursor == 18){;}
            // if cursor is on prompt and input is correct:
            else if(input == prompts[cursor - 2]){
                prompts[cursor - 2] = 20; // change prompt to '$'
                p_count--; // decrease prompt count
                score++; // increase score
            }
        }
        
        cursor = do_ADC() / (1023.0 / 60.0); // get new cursor position
        
        // below code creates gray areas in potentiometer position, thus 
        // preventing cursor from jittering between neighboring spots
        
        // if cursor has moved substantially, take as is
        if(cursor - 3 > cursor_x * 3 || cursor + 3 < cursor_x * 3){
            cursor = cursor / 3;
            cursor_x = cursor;
        }
        else{
            // if cursor is not in a gray area, take as is
            if(cursor % 3 == 0){
                cursor = cursor / 3;
                cursor_x = cursor;
            }
            // if cursor is in a gray area, do not update position 
            else{
                cursor = cursor_x;
            }
        }
        
        disp_play(prompts, cursor); // update display
        delay_ms(20); // game runs smoother, and debounce is easier to filter
    }
    return 0;
}
