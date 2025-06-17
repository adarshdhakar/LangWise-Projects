# GC_24_ARDUGEM_38
## Minesweeper Game
This project implements the classic Minesweeper game using an Arduino and a TFT display. The game allows players to uncover tiles on a grid while avoiding hidden mines.

### Hardware Requirements
- Arduino board
- TFT display (compatible with Adafruit_GFX and Adafruit_ST7735 libraries)
- Joystick module or buttons for input

### Software Requirements
- Arduino IDE
- Adafruit_GFX library
- Adafruit_ST7735 library

### Installation
1. Connect the hardware components according to the provided pin configurations in the code.
2. Install the required libraries in your Arduino IDE.
3. Upload the code to your Arduino board.

### Usage
1. Upon starting the game, the main menu will be displayed on the TFT screen.<br>
 <img src="image/mainmenu.jpg" alt="MainMenu" height="350" width="275">
2. Press the "B" button in the joystick module to begin the game or Press the "D" button to go to the rules page.<br>
3. Use the joystick to move the cursor on the grid. Press “A” button to dig or uncover the hidden cell or press “C” button to flag the cell.
  <div height="350" width = "750">
  <img src="image/grid.png" alt="Grid" height="350" width="250"><img src="image/open.png" alt="Open" height="350" width="250"><img src="image/flag.png" alt="Flag" height="350" width="250"> 
  </div>
4. Avoid uncovering tiles containing mines.<br>
5. Use flags to mark potential mine locations.<br>
6. Win the game by uncovering all non-mine tiles, or lose by uncovering a mine.<br>
 <div height="350" width = "500">
  <img src="image/lose.png" alt="Lose" height="350" width="250"><img src="image/win.png" alt="Win" height="350" width="250"> 
  </div>
7. Your score will be displayed at the end of the game.<br>
8. Now again Press button “B” to go to Main Menu to play again.<br>

### Features
1. Main menu with options to start the game or view rules.<br>
 <img src="image/mainmenu.jpg" alt="MainMenu" height="350" width="275">
2. Rules section explaining the game mechanics.<br>
 <div height="350" width = "750">
  <img src="image/rule1.png" alt="Rule" height="350" width="250"><img src="image/rule2.png" alt="Rule" height="350" width="250"><img src="image/rule3.png" alt="Rule" height="350" width="250"> 
  </div>
3. Dynamic grid drawing on the TFT screen.<br>
<img src="image/grid.png" alt="Grid" height="350" width="250">
4. Bomb and flag graphics for visual representation.<br>
 <div height="350" width = "500">
  <img src="image/bomb.png" alt="Bomb" height="350" width="250"><img src="image/flag.png" alt="Flag" height="350" width="250"> 
  </div>
5. Endgame screen displaying win/loss status and score and option to return to the main menu after finishing a game.<br>
 	 <div height="350" width = "500">
  <img src="image/lose.png" alt="Lose" height="350" width="250"><img src="image/win.png" alt="Win" height="350" width="250"> 
  </div>

### Credits
- This project is inspired by the classic Minesweeper game.
- Libraries used: Adafruit_GFX, Adafruit_ST7735.

### Notes
- You can customize the game parameters such as grid size and number of mines in the code according to your preferences by changing the cell size but keep in mind the computational power of Arduino UNO.

<video width="640" height="360" controls>
  <source src="Minesweeper.mp4" type="video/mp4">
  Your browser does not support the video tag.
</video>
