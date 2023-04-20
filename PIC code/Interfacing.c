/* 
 * File:   Interfacing.c
 * Author: ENCM 511 Group 20
 *
 * Created on November 28, 2022, 4:02 PM
 */

#include "Interfacing.h"
#include "stdlib.h"
#include "xc.h"
#include "IOs.h"
#include "TimeDelay.h"
#include <p24fxxxx.h>
#include <p24F16KA101.h>
#include "UART2.h"
#include "macros.h"

extern int CNflag;
extern int RXflag;

/// Waits for input while player is on title screen, and triggers
/// appropriate response to input
int title(void)
{
    releasePBs(0);  // Ensure buttons do not start pressed
    IOcheck(); // validate next button press
    waitIO();  // Idle until input is detected
    int input; 
    
    while(1){
        input = IOcheck(); // sample input
        if(input == 6){ // if all buttons pressed:
            XmitUART2('E',1); // tell display program to terminate
            return 0;
        }
        else if(input != -1){ // if other valid input is detected:
            XmitUART2('S',1); // tell display program to move to level select
            while(readByte() != 'S'){;} // wait for confirmation
            return 1;
        }
        delay_ms(30); // create time between input samples
    }
}

/// Sends the location of the cursor and prompts in play, to the
/// display program
void disp_play(int* targs, int cursor)
{
    for(int i=0; i < 16; i++){
        // skip spots with no prompt
        if(targs[i] < 0 || targs[i] == 10) continue;
        // show '$' for a few moments after prompt is successfully clicked:
        if(targs[i] > 10){ 
            targs[i]--;
            XmitUART2(135 + 8*i, 1);
            continue;
        }
        XmitUART2(targs[i] + 128 + 8*i, 1); // send symbol and position
    }
    XmitUART2(cursor + 32, 1);
    while(readByte() != 'D'){;} // wait for confirmation
    
    return;
}

/// Tells display program to go to pause screen, then waits for input.
int pause_menu(void)
{
    XmitUART2(17,1);
    releasePBs(0);  // Ensure buttons do not start pressed
    int in = IOcheck(); // validate next button press
    waitIO();  // Idle until input is detected
    
    while(in < 0){ // loop until valid input is detected
        in = IOcheck(); // sample input
        delay_ms(30); // create time between input samples
    }
    return in; // return input
}

/// Sends commands for navigating level select screen to display program.
/// After level has been selected, function reads and stores level data.
int load_level(Prompt* l_data)
{
    int input;
    int rx_in;
    
    // sending commands:
    while(1){
        input = IOcheck();
        if(input == 0 || input == 2){ // move left or right
            XmitUART2(input + 49,1);
            continue;
        }
        if(input == 1){ // select level
            XmitUART2(input + 49,1);
            break;
        }
        delay_ms(30);
    }
    
    // reading and storing level data:
    int i = 0;
    XmitUART2('L',1); // indicate that PIC is ready to receive data
    while(readByte() != 'L'){;} // wait for confirmation
    while(1){
        rx_in = readByte(); // read first byte of two
        if(rx_in == 48) break; // break if all data has been sent
        l_data[i].symbol = rx_in & 0x07; // parse prompt symbol from byte one
        l_data[i].position = (rx_in & 0x78) / 8; // parse prompt position
        XmitUART2(rx_in,1); // send confirmation
        rx_in = readByte(); // read second byte of two
        l_data[i].arrival = rx_in - 128; // parse arrival time of prompt
        XmitUART2(rx_in,1); // send confirmation
        i++; // increment index of l_data to store prompt data in
    }
    XmitUART2('P',1); // indicate that PIC is ready to move to gameplay 
    while(readByte() != 'P'){;} // wait for confirmation
    return i; // return number of prompts stored
}
