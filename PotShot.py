# -*- coding: utf-8 -*-
"""

APP PROJECT 2
Created on Sun Nov 27 12:07:04 2022

@author: Group 20
@author: Adam Pezoulas
@author: Ethan Reed
"""

import serial 
import time
import sys
import pandas as pd
from os import path

MAX_L = 4

USAGE = "Usage: python3 App2.py serial_port\n- serial_port: name of serial port connected to PIC microcontroller"


def println(stringToPrint):
    print("\r |___________________________|   |__{{}_vV____[" + stringToPrint + "]___wY_{}}_v_|  ", end='')
    sys.stdout.flush()

def printScore(score, total):
    result = ""
    ratio = int((score * 10) / total)

    for i in range(3):
        if ratio >= (i + 1) * 3:
            result += '*'
        else:
            result += '-'

    scoreStr = str(int(score/10)) + str(score%10)
    totalStr = str(int(total/10)) + str(total%10)

    result += " Score: " + scoreStr + '/' + totalStr + ' '

    for i in range(3):
        if ratio >= (3 - i) * 3:
            result += '*'
        else:
            result += '-'

    println(result)

def transition(stateCode):
    return

def main():
    #print usage message if the script was run with to few/to many args
    if len(sys.argv) != 2:
        print("\nProgram halted, improper usage.\n") 
        print(USAGE + '\n')
        return
    
    #try to open serial port to talk to PIC; print usage message if unable
    try:
        ser = serial.Serial(port = sys.argv[1], baudrate = 9600, bytesize = 8, timeout = 1, stopbits = serial.STOPBITS_ONE)
    except:
        print("\nProgram halted, could not find port \""+sys.argv[1]+"\"\n") 
        print(USAGE + '\n')
        return

    levelDir = path.realpath(path.dirname(__file__))
    levelDir = levelDir + '/Levels'

    #state variables and game data
    state = "wait"
    level = 1
    score = 0
    levelData = None

    # Display gameplay information:

    print("\n  ___________________________     ___________________")
    print(" |                           |   |                   |        ___")
    print(" |   HOW TO PLAY:            |   |   LEGEND:         |       |   |")
    print(" |                           |   |                   |       |   |")
    print(" | - From title screen press |   |   1 : ._..=..=.   |       | G |")
    print(" |   all three buttons to    |   |                   |       | A |")
    print(" |   exit the python program |   |   2 : .=.._..=.   |       | M |")
    print(" |                           |   |                   |       | E |")
    print(" | - Press any one button to |   |   3 : .=..=.._.   |       |   |")
    print(" |   enter level selection   |   |                   |       |   |")
    print(" |                           |   |   L : ._.._..=.   |       | W |")
    print(" | - Once the level begins,  |   |                   |       | I |")
    print(" |   characters will start   |   |   R : .=.._.._.   |       | N |")
    print(" |   appearing in the play   |   |                   |       | D |")
    print(" |   area.  1 2 3 L R ? !    |   |   ? : ._..=.._.   |       | O |")
    print(" |                           |   |                   |       | W |")
    print(" | - Using your trusty pot,  |   |   ! : ._.._.._.   |       |   |")
    print(" |   move your cursor over   |   |                   |     __|   |__")
    print(" |   the symbols and press   |   |   ._. = pressed   |    \ *     * /")
    print(" |   the correct combo of    |   |                   |     \ *   * /")
    print(" |   buttons to score points |   |   .=. = not :)    |      \ * * /")
    print(" |                           |   |___________________|       \ * /")
    print(" | - At any point during     |                                \_/")                            
    print(" |   play, you can move the  |    ______________________________________________")
    print(" |   cursor outside the play |   |                        _____                 |")
    print(" |   area and press any      |   |     \___/             (___)_)                |")
    print(" |   button to pause the     |   |   __/  _\_____                        ____   |")
    print(" |   game. Press (1) to quit |   |    _\_(_   ___)     ___          ____(  __)__|")
    print(" |   to menu, any other      |   |   (______)_)      _(  _)_       (______(_____|")
    print(" |   will resume play        |   |                  (____)__)                   |")
    print(" |                           |   |      _____                                   |")
    print(" | - From the score display  |   |     (_(__)) ____/\________/\____       .u.   |")
    print(" |   screen, press a button  |   |            /  [|__|] _= [|__|] _\      {}}   |")
    print(" |   to return to the start  |   |   w       /______________________\     {}}   |")

    ser.reset_input_buffer()

    while (1):
        time.sleep(0.05)
    
    
        if state == "wait":     #Wait for an input from the PIC to start
            println("  | +|    POT SHOT  ")
            while ser.inWaiting() < 1: pass    #Echos the received byte when it gets it
            echo = ser.read(1)    #Go to stage select
            if echo == b'S':
                ser.write(b'S')
                state = "select"
            elif echo == b'E':
                break 

    
        if state == "select":       #Stage select screen
            println("<-1-  Lvl." + str(level) + "(2)  -3->")
            while ser.inWaiting() < 1: pass   #check for PIC input
            pressed = ser.read(1)
            if pressed == b'1' and level != 1:     #Decrease level if it wouldn't go below 1
                level -= 1
            elif pressed == b'3' and level != MAX_L:        #Increase the level, won't go above a max
                level += 1
            elif pressed == b'2':        #Moves to a state where it waits for the MCU to start loading
                levelPath = levelDir + '/L' + str(level) + '.xlsx'
                levelData = pd.read_excel(levelPath)
                state = "waitLoad"
            elif pressed == b'\x00':
                state = "wait" 
                

        if state == "waitLoad":
            println("  Loading Level...  ")
            while(ser.inWaiting() < 1): pass
            echo = ser.read(1)      #Start loading level onto PIC if it confirms it's ready
            if echo == b'L':
                ser.write(b'L')
                state = "loadLevel"
            elif echo == b'\x00':
                state == "wait"
                
                
                
        if state == "loadLevel":
            for i in range(len(levelData.index)):     #send level data
                ser.write(bytes([levelData.at[i,'byteOne']]))
                while (ser.inWaiting() < 1): pass
                ser.read(1)
                ser.write(bytes([levelData.at[i,'byteTwo']]))
                while (ser.inWaiting() < 1): pass
                ser.read(1)

            counter = 10    #reset counter
            state = "waitPlay"#wait for confirmation from PIC
            ser.write(b'0')
        

        
        if state == "waitPlay":
            if ser.inWaiting() >= 1:
                echo = ser.read(1)      #Start loading level onto PIC if it confirms it's ready
                if echo == b'P':
                    ser.write(b'P')
                    state = "play"
                elif echo == b'\x00':
                    state = "wait"
                
        
        if state == "play":
            playField = list(" [                ] ") #make a list of the screen to print
            readyToUpdate = 0       #tracks when all the symbols and cursor have been read
            while readyToUpdate == 0:       #Relies on the cursor being sent last
                while ser.inWaiting() < 1: pass
                readByte = int.from_bytes(ser.read(1), 'big')
                
                if readByte & 0b10000000 > 0:   #if byte is for a symbol
                    position  = (readByte & 0b01111000) >> 3
                    symbol = readByte & 0b00000111
                    
                    if symbol == 0: charToShow = '1'
                    if symbol == 1: charToShow = '2'
                    if symbol == 2: charToShow = '3'
                    if symbol == 3: charToShow = 'L'
                    if symbol == 4: charToShow = 'R'
                    if symbol == 5: charToShow = '?'
                    if symbol == 6: charToShow = '!'
                    if symbol == 7: charToShow = '$'
                
                    playField[position + 2] = charToShow
                    
                
                if readByte & 0b10000000 == 0:  #if byte is for the cursor, pause, or end
                    if readByte == 0b00010001:
                        state = "pause"
                        break
               
                    elif (readByte & 0b01000000) == 0b01000000:
                        score = readByte & 0b00111111
                        state = "end"
                        break

                    elif readByte == 0:
                        state = "wait"
                        break   

                    else:
                        position = readByte & 0b00011111
                           
                        #Print the cursor elements if they aren't out of bounds or on an existing symbol
                        if position == 0 or position == 19: playField[position] = 'P'
                        for i in range(20):
                            if playField[i] != ' ': continue
                            if i == position - 1: playField[i] = '>'
                            elif i == position: playField[i] = '_'
                            elif i == position + 1: playField[i] = '<'
                       
                        readyToUpdate = 1

            println(''.join(playField))
            ser.write(b'D')
        
        
        if state == "pause":
            println(" Press (1) to Quit. ")   #Pause screen
            while ser.inWaiting() < 1: pass
            echo = ser.read(1)  #Return to play or quit back to menu
            if echo == b'\x00':
                state = "wait"
            else:
                state = "play"

    
        if state == "end":
            printScore(score, len(levelData.index))
            while ser.inWaiting() < 1: pass
            ser.read(1)
            #reset level?
            state = "wait"
            #end screen
    
    print("\nTERMINATED.")

if __name__ == "__main__" :
    main()