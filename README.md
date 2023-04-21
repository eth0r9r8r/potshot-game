# potshot-game
## Preface
This project was created with the assistance of a classmate for the class ENCM 511 at the University of Calgary.
All files were created by me, Ethan Reed, with the exception of `App2.py` and `PotShot.py`.

`PotShot.py` is a version of `App2.py` that I modified. I included both `PotShot.py` and `App2.py` to differentiate between the lines of
code written by me and the lines of code written by my classmate.

## What it Does and How it Does it
Below is the game as it appears on the command-line when the file `PotShot.py` is run. This includes the gameplay instructions,
some ascii art, and the game itself (which is displayed on only a single line).
```
 ___________________________     ___________________
|                           |   |                   |        ___
|   HOW TO PLAY:            |   |   LEGEND:         |       |   |
|                           |   |                   |       |   |
| - From title screen press |   |   1 : ._..=..=.   |       | G |
|   all three buttons to    |   |                   |       | A |
|   exit the python program |   |   2 : .=.._..=.   |       | M |
|                           |   |                   |       | E |
| - Press any one button to |   |   3 : .=..=.._.   |       |   |
|   enter level selection   |   |                   |       |   |
|                           |   |   L : ._.._..=.   |       | W |
| - Once the level begins,  |   |                   |       | I |
|   characters will start   |   |   R : .=.._.._.   |       | N |
|   appearing in the play   |   |                   |       | D |
|   area.  1 2 3 L R ? !    |   |   ? : ._..=.._.   |       | O |
|                           |   |                   |       | W |
| - Using your trusty pot,  |   |   ! : ._.._.._.   |       |   |
|   move your cursor over   |   |                   |     __|   |__
|   the symbols and press   |   |   ._. = pressed   |    \ *     * /
|   the correct combo of    |   |                   |     \ *   * /
|   buttons to score points |   |   .=. = not :)    |      \ * * /
|                           |   |___________________|       \ * /
| - At any point during     |                                \_/
|   play, you can move the  |    ______________________________________________
|   cursor outside the play |   |                        _____                 |
|   area and press any      |   |     \___/             (___)_)                |
|   button to pause the     |   |   __/  _\_____                        ____   |
|   game. Press (1) to quit |   |    _\_(_   ___)     ___          ____(  __)__|
|   to menu, any other      |   |   (______)_)      _(  _)_       (______(_____|
|   will resume play        |   |                  (____)__)                   |
|                           |   |      _____                                   |
| - From the score display  |   |     (_(__)) ____/\________/\____       .u.   |
|   screen, press a button  |   |            /  [|__|] _= [|__|] _\      {}}   |
|   to return to the start  |   |   w       /______________________\     {}}   |
|___________________________|   |__{{}_vV____[  | +|    POT SHOT  ]___wY_{}}_v_|
```

The game is run for the most part on a PIC24F16KA101 microcontroller, which is programmed with the C files found in
the directory `PIC code` using MPLAB and a PICkit. This microcontroller handles input from three push buttons and
a potentiometer (hence potshot), which are used to play the game. The microcontroller also keeps track of the game's
state, handles the game's logic, and sends data to the Python program for rendering.

The Python program (either `PotShot.py` or `App2.py`) communicates with the microcontroller via UART serial communications;
it aids in the game set-up, sends data from the selected level to the controller, then decodes and renders the data that it
recieves. This data contains periodic information about the game's state, which is encoded and sent by the controller.

Level information is saved in excel files, read and encoded by the Python program, and then sent to the microcontroller.

## Demonstration
Link to youtube demo [here](https://www.youtube.com)
