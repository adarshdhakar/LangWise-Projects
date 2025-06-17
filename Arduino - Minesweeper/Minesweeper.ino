#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

#define TFT_CS 10
#define TFT_RST 12
#define TFT_DC 9

// Color definitions
#define BLACK   0x0000
#define WHITE   0xFFFF
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define ORANGE  0xFD60
#define PURPLE  0x801F
#define BROWN   0x8200

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128

#define CELL_SIZE 20  // Size of each cell in pixels

#define NUM_ROWS (SCREEN_HEIGHT / CELL_SIZE)
#define NUM_COLS (SCREEN_WIDTH / CELL_SIZE)

#define NUM_MINES 7  // Number of mines in the game
int bomb_no_mines=0;
int game_over = 0;

int board_count = 0;
int revealed_count = 0;

int score = 10000;
int highestScore = 0;

int up_button = 2;
int down_button = 4;
int left_button = 5;
int right_button = 3;
int start_button = 6;
int select_button = 7;
int analog_button = 8;
int x_axis = A0;
int y_axis = A1;
int buttons[] = {up_button, down_button, left_button, right_button, start_button, select_button, analog_button};

int board[NUM_ROWS][NUM_COLS];     // Mines board
int revealed[NUM_ROWS][NUM_COLS];  // Revealed cells board

int cursorRow = 0;
int cursorCol = 0;

void setup() {

  for(int i; i < 7; i++){
    pinMode(buttons[i], INPUT);
    digitalWrite(buttons[i], HIGH);
  }

  unsigned long a = analogRead(0);
  srand(a);

  Serial.begin(9600);
  tft.initR(INITR_GREENTAB);

  bomb_no_mines = 0;
  board_count = 0;
  revealed_count = 0;
  score = 10000;
  mainScreen();
}

void loop() {
  // Main game loop
  handleInput();
  delay(300);
}

void mainScreen(){
  tft.setRotation(0);  // Adjust rotation if needed

  // Fill the screen with a background color
  tft.fillScreen(BLACK);
  int left = 0; 
  int right = 0;
  while (1) {
    int xPos = tft.width();  // Start off-screen (right edge)

    mainScreenComp();

    while (xPos > -tft.width()) {
      // Clear only the area where the scrolling text will be
      tft.fillRect(xPos, 0, tft.width() + xPos+200, 40, BLACK);

      // Display scrolling Minesweeper title
      tft.setTextSize(2);
      tft.setCursor(xPos, 20);
      tft.setTextColor(ST77XX_YELLOW);
      tft.print("Minesweeper"); 

      xPos--;     // Move text to the left
      left = digitalRead(left_button);
      right = digitalRead(right_button);
      delay(50);
      if (left == 0) {
        tft.fillScreen(BLACK);

        drawButton2(1);
        delay(700);

        rules();
        delay(100);
        tft.fillScreen(BLACK);
        mainScreenComp();
      }
      else if(right == 0){
        tft.fillScreen(BLACK);
        drawButton(27, 95, 80, 30, GREEN, "START >>", 1);
        delay(700);
        // tft.setRotation(3);
        initializeGame();
        break;
      }
      delay(0);
    }
    if(right == 0){
      break;
    }
  }
  delay(10);
}

void mainScreenComp(){
  tft.setTextWrap(false);
  
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(5, 50);
  tft.println("Press START to begin");

  drawButton2(0);

  drawButton(27, 95, 80, 30, GREEN, "START >>", 0);
}

void drawButton2(int check){
  if(check == 1){
    tft.setTextSize(2);
    tft.setCursor(0, 20);
    tft.setTextColor(ST77XX_YELLOW);
    tft.print("Minesweeper"); 

    mainScreenComp();
  }
  tft.drawRoundRect(31, 69, 70, 18, 7, WHITE);
  tft.fillRoundRect(32, 70, 68, 16, 7, MAGENTA);

  tft.setCursor(40, 75);
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.println("<<");
  tft.setCursor(41, 75);
  tft.print("<<");

  if(check == 0)
  tft.setTextColor(WHITE);

  else tft.setTextColor(BLACK);

  tft.setCursor(54, 75);
  tft.print(" Rules");
  if(check == 1){
    tft.setCursor(54, 74);
    tft.print(" Rules");
  }
  
  // tft.setCursor(5, 75);
  // tft.setTextSize(1);
  // tft.setTextColor(WHITE);
  // tft.println("<<");
  // tft.setCursor(6, 75);
  // tft.print("<<");
  // tft.print(" Rules");
}
void drawButton(int x, int y, int width, int height, uint16_t color, const char* label, int check) {
  if(check == 1){
    tft.setTextSize(2);
    tft.setCursor(0, 20);
    tft.setTextColor(ST77XX_YELLOW);
    tft.print("Minesweeper"); 
    mainScreenComp();
  }

  tft.fillRoundRect(x, y, width, height, 20, color);
  tft.drawRoundRect(x, y, width, height, 20, WHITE);

  int labelWidth = strlen(label) * 6;  // Approximate width based on character size

  int labelX = x + (width - labelWidth) / 2;
  int labelY = y + (height - 8) / 2;  // 8 is the text height for the default font

  if(check == 0)
  tft.setTextColor(BLACK);

  else tft.setTextColor(WHITE);

  tft.setTextSize(1);

  tft.setCursor(labelX, labelY);
  tft.println(label);

  tft.setCursor(labelX, labelY+1);
  tft.println(label);
}

void rules(){
  tft.setTextWrap(true);
  pinMode(up_button, INPUT);
  digitalWrite(up_button, HIGH);

  pinMode(down_button, INPUT);
  digitalWrite(down_button, HIGH);

  page1();
  int curr = 1;

  // Display the rules content
  while(1){
    int up = digitalRead(up_button);
    int down = digitalRead(down_button);
    // int left = digitalRead(left_button);
    int right = digitalRead(right_button);

    delay(50);
    if(up == 0){
      // if(curr == 1){
      //   page1();
      //   curr = 1;
      // }
      if(curr == 2){
        page1();
        curr--;
      }
      else if(curr == 3){
        page2();
        curr--;
      }
    }
    if(down == 0){
      if(curr == 1){
        page2();
        curr++;
      }
      else if(curr == 2){
        page3();
        curr++;
      }
    }
    // if(left == 0){
    //   mainScreen();
    // }
    if(right == 0){
      return;
    }
    // if(next == 0){
    //   tft.fillScreen(BLACK);
    // }
  }
}

void page1(){
  tft.fillScreen(ST7735_BLACK);
  tft.setTextSize(0);
  tft.setTextColor(ST7735_WHITE);
  tft.setCursor(50, 2);

  tft.println("RULES");
  tft.setCursor(50, 3);
  tft.print("RULES");

  tft.println();
  tft.println("1.This is a single player game consisting of 10x10 grid.");
  tft.println();
  tft.println("2.The grid is covered with tiles and some of them conceal hidden mines beneath.");
  tft.println();
  tft.println("3.The goal is to uncover all the tiles that do not contain mines, without detonating any of the mines.");
}
void page2(){
  tft.fillScreen(ST7735_BLACK);
  tft.setTextSize(0);
  tft.setTextColor(ST7735_WHITE);
  tft.setCursor(0, 5);

  tft.println();
  tft.println("4.The game is won when all non-mine tiles are revealed.");
  tft.println();
  tft.println("5.If the selected tile contains a mine, you lose the game.");
  tft.println();
  tft.println("6.When a tile is selected, it will reveal a number, indicating how many mines are present in its surrounding 8 tiles.");
}
void page3(){
  tft.fillScreen(ST7735_BLACK);
  tft.setTextSize(0);
  tft.setTextColor(ST7735_WHITE);
  tft.setCursor(0, 5);

  tft.println();
  tft.println("8.If a tile does not have any adjacent mines, it will automatically uncover all its neighbouring tiles until it reaches tiles with adjacent mines.");
  tft.println();
  tft.println("9.To help identify potential mine locations, you can place flags on tiles that you suspect contain mines.");
  tft.println();
}

void bombs_placement_mines(){
  for(int i=0;i<NUM_ROWS;i++){
    for(int j=0;j<NUM_COLS;j++){
      if(board[i][j]!=9) {
        int count=0;
        if((i>0&&i<NUM_ROWS-1)&&(j>0&&j<NUM_COLS-1)){
          if(board[i-1][j-1]==9) count++;
          if(board[i-1][j]==9) count++;
          if(board[i-1][j+1]==9) count++;
          if(board[i][j-1]==9) count++;
          if(board[i][j+1]==9) count++;
          if(board[i+1][j-1]==9) count++;
          if(board[i+1][j]==9) count++;
          if(board[i+1][j+1]==9) count++;
        }else if((i==0)&&(j>0&&j<NUM_COLS-1)){
          if(board[i][j-1]==9) count++;
          if(board[i][j+1]==9) count++;
          if(board[i+1][j-1]==9) count++;
          if(board[i+1][j]==9) count++;
          if(board[i+1][j+1]==9) count++;
        }else if((i==NUM_ROWS-1)&&(j>0&&j<NUM_COLS-1)){
          if(board[i-1][j-1]==9) count++;
          if(board[i-1][j]==9) count++;
          if(board[i-1][j+1]==9) count++;
          if(board[i][j-1]==9) count++;
          if(board[i][j+1]==9) count++;
        }else if((j==NUM_COLS-1)&&(i>0&&i<NUM_ROWS-1)){
          if(board[i-1][j-1]==9) count++;
          if(board[i-1][j]==9) count++;
          if(board[i][j-1]==9) count++;
          if(board[i+1][j-1]==9) count++;
          if(board[i+1][j]==9) count++;
        }else if((j==0)&&(i>0&&i<NUM_ROWS-1)){
          if(board[i-1][j]==9) count++;
          if(board[i-1][j+1]==9) count++;
          if(board[i][j+1]==9) count++;
          if(board[i+1][j]==9) count++;
          if(board[i+1][j+1]==9) count++;
        }else if((i==0)&&(j==0)){
          if(board[i+1][j]==9) count++;
          if(board[i][j+1]==9) count++;
          if(board[i+1][j+1]==9) count++;
        }else if((i==0)&&(j==NUM_COLS-1)){
          if(board[i+1][j]==9) count++;
          if(board[i][j-1]==9) count++;
          if(board[i+1][j-1]==9) count++;
        }else if((i==NUM_ROWS-1)&&(j==0)){
          if(board[i-1][j]==9) count++;
          if(board[i][j+1]==9) count++;
          if(board[i-1][j+1]==9) count++;
        }else if((i==NUM_ROWS-1)&&(j==NUM_COLS-1)){
          if(board[i-1][j]==9) count++;
          if(board[i][j-1]==9) count++;
          if(board[i-1][j-1]==9) count++;
        }
        board[i][j]=count;    
        if(board[i][j] >= 1 && board[i][j] <= 8){
          board_count ++;
        }
      }
    }
  }
}

void initializeGame() {
  // Clear the board
  memset(board, 0, sizeof(board));
  memset(revealed, 0, sizeof(revealed));
  
  while (bomb_no_mines < NUM_MINES) {
    for (int i = 0; i < NUM_ROWS; i++) {
      for (int j = 0; j < NUM_COLS; j++) {
        int q = rand();
        if (q % 8 == 0 && bomb_no_mines < NUM_MINES && board[i][j] == 0) {
          board[i][j] = 9;
          bomb_no_mines++;
        }
      }
    }
  }
  bombs_placement_mines();
  Serial.println();
  for (int i = 0; i < NUM_ROWS; i++) {
    for (int j = 0; j < NUM_COLS; j++) {
      Serial.print(board[i][j]);
      Serial.print(" ");
    }
    Serial.println();
  }
  Serial.println(board_count);
  drawBoard(cursorRow, cursorCol);
}

void bomb(int rowb,int colb){
  int x = rowb+4 + CELL_SIZE/2;
  int y = colb+7 + CELL_SIZE/2;
  // int centerX = (rowb + x)/2;
  // int centerY = (colb + y)/2;
  tft.fillCircle(x, y, 5, BLACK);
  tft.fillRect(x, y-9, 2, 4,RED);
  tft.fillRect(x-2, y-3, 4, 6,RED);
  tft.setTextSize(0);
  tft.setTextColor(WHITE);
  tft.setCursor(x-2, y-3);
  tft.println("B");
}

void flag(int rowf, int colf){
  rowf += 11;
  colf += 9;
  tft.fillTriangle(rowf, colf, rowf, colf+7, rowf+8, colf+3, RED);
  tft.drawTriangle(rowf, colf, rowf, colf+7, rowf+8, colf+3, BLACK);
  tft.fillRect(rowf, colf, 2, 12, BLACK);
}

void drawBoard(int crow, int ccol) {
  tft.fillScreen(ST7735_GREEN);  // Clear the screen
  tft.drawRect(2, 4, NUM_ROWS*CELL_SIZE+4, NUM_COLS*CELL_SIZE+4, BLUE);
  tft.drawRect(3, 5, NUM_ROWS*CELL_SIZE+2, NUM_COLS*CELL_SIZE+2, BLUE);
  // Draw cells
  for (int row = 0; row < NUM_ROWS; row++) {
    for (int col = 0; col < NUM_COLS; col++) {
      int x = col * CELL_SIZE;
      int y = row * CELL_SIZE;
      int color = revealed[row][col] ? ST7735_WHITE : ST7735_BLUE;  
      tft.drawRect(x+4, y+6, CELL_SIZE, CELL_SIZE, color);              

      tft.setCursor(x+13, y+12);
      tft.setTextSize(1);
      if(revealed[row][col] != 0 && revealed[row][col] != -1 && revealed[row][col] != 9 && revealed[row][col] != 10){
        if(revealed[row][col] == 1) tft.setTextColor(MAGENTA);
        else if(revealed[row][col] == 2) tft.setTextColor(PURPLE);
        else if(revealed[row][col] == 3) tft.setTextColor(BROWN);
        else tft.setTextColor(BLACK);

        tft.print(revealed[row][col]);
        tft.setCursor(x+14,y+12);
        tft.print(revealed[row][col]);
      }
      else if(revealed[row][col] == 9){
        bomb(x, y);
      }else if(revealed[row][col]==10){
        flag(x,y);
      }
      if (row == crow && col == ccol) {
        tft.drawRect(x + 5, y + 7, CELL_SIZE - 2, CELL_SIZE - 2, ST7735_RED);
        tft.drawRect(x + 4, y + 6, CELL_SIZE , CELL_SIZE , ST7735_RED);
      }
    }
  }
}

void open_board_mines(int start,int end){
  if((start>=0&&start<=NUM_ROWS-1)&&(end>=0&&end<=NUM_COLS-1)&&(revealed[start][end]==0  || revealed[start][end] == 10)){      
    drawBoard(start,end);
    cursorRow = start;
    cursorCol = end;
    if(board[start][end]>=1&&board[start][end]<=8){
      revealed[start][end]=board[start][end];
      drawBoard(start,end);
      revealed_count ++;
      score += 211;
    } 
    else if(board[start][end]==0){
      revealed[start][end]=-1;
      open_board_mines(start-1,end-1);
      open_board_mines(start-1,end);
      open_board_mines(start-1,end+1);
      open_board_mines(start,end-1);
      open_board_mines(start,end+1);
      open_board_mines(start+1,end-1);
      open_board_mines(start+1,end);
      open_board_mines(start+1,end+1);
    }
    else if(board[start][end]==9){
            game_over=1;
            revealed[start][end]=9;
            drawBoard(start,end);
            for(int i=0;i<NUM_ROWS;i++){
                for(int j=0;j<NUM_COLS;j++){
                    if(board[i][j]==9){
                        revealed[i][j]=9;
                        drawBoard(i,j);
                    }
                }
            }
            delay(2000);
            score -= 7000;
            endPage(false);
            Serial.print("You Lost The Game\n");
            return;
        }
    }else{
        return;
    }
}

void endPage(bool isWin) {
  // Clear the screen
  tft.fillScreen(BLACK);

  // Display "Game Over" text
  tft.setTextSize(2);
  
  if (isWin) {
    // tft.setCursor(20, 10);
    // tft.setTextColor(GREEN);
    // tft.println("You Win");
    endScreenComp();
    static bool blink = false;
    while(1){
      blink = !blink;

      // Display "Game Over" text
      tft.setTextSize(2);
      tft.setTextColor(blink ? GREEN : BLACK); // Toggle color for blinking effect
      tft.setCursor(20, 30);
      tft.println("You Win");
      delay(300);
      
      int left = digitalRead(left_button);
      int right = digitalRead(right_button);
      
      if(right == 0){
        delay(200);
        setup();
        return;
      }
    }
  } else {
    tft.setTextSize(1);
    tft.setCursor(40, 50);
    tft.setTextColor(RED);
    tft.println("You Lose");
    endScreenComp();
    static bool blink = false;
    while(1){
      blink = !blink;

      // Display "Game Over" text
      tft.setTextSize(2);
      tft.setTextColor(blink ? RED : BLACK); // Toggle color for blinking effect
      tft.setCursor(5, 27);
      tft.println("Game Over!");
      delay(300);
      
      int left = digitalRead(left_button);
      int right = digitalRead(right_button);
    
      if(right == 0){
        delay(200);
        setup();
        return;
      }
    }
  }
}

void endScreenComp(){
  // Display score
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(0, 65);
  tft.print("Score: ");
  tft.println(score);

  // Update highest score if necessary
  if (score > highestScore) {
    highestScore = score;
  }

  // Display highest score
  tft.setCursor(0, 78);
  tft.print("Highest Score: ");
  tft.println(highestScore);

  drawButton4("MAIN MENU >>");
}

void drawButton4(const char* label){
  tft.fillRoundRect(22, 97, 84, 20, 8, YELLOW);
  tft.drawRoundRect(21, 96, 86, 22, 10, BLACK);

  int labelWidth = strlen(label) * 6; // Approximate width based on character size
  int width = 84;
  int height = 20;
  int x = 22;
  int y = 97;
  int labelX = x + (width - labelWidth) / 2;
  int labelY = y + (height - 8) / 2; // 8 is the text height for the default font

  tft.setCursor(labelX, labelY);
  tft.setTextColor(BLUE);
  tft.setTextSize(1);
  tft.println(label);

  tft.setCursor(labelX, labelY-1);
  tft.println(label);
}

void handleInput() {
  int horizontal = analogRead(x_axis);  // Read X-axis of joystick
  int vertical = analogRead(y_axis);    // Read Y-axis of joystick

  int check = 0;

  if (horizontal < 100) {
    cursorCol--;
    if (cursorCol < 0) cursorCol = 0;
    check = 1;
    score -= 7;
  } else if (horizontal > 600) {
    cursorCol++;
    if (cursorCol >= NUM_COLS) cursorCol = NUM_COLS - 1;
    check = 1;
    score -= 11;
  }

  if (vertical > 600) {
    cursorRow--;
    if (cursorRow < 0) cursorRow = 0;
    check = 1;
    score -= 13;
  } else if (vertical < 100) {
    cursorRow++;
    if (cursorRow >= NUM_COLS) cursorRow = NUM_ROWS - 1;
    check = 1;
    score -= 10;
  }
  
  if (digitalRead(up_button) == LOW) {
    open_board_mines(cursorRow, cursorCol);
    Serial.println(revealed_count);
    if(board_count == revealed_count){
      Serial.println("You win.");
      delay(2000);
      endPage(true);
    }
  }
  else if(digitalRead(down_button) == LOW && revealed[cursorRow][cursorCol] == 0){
    Serial.println(cursorRow);
    Serial.println(cursorCol);
    revealed[cursorRow][cursorCol] = 10;
    if(revealed[cursorRow][cursorCol]==board[cursorRow][cursorCol]+1) score+=347;
    else score-=264;
    int x = cursorCol * CELL_SIZE;
    int y = cursorRow * CELL_SIZE;
    flag(x,y);
  }

  if(check == 1)drawBoard(cursorRow, cursorCol);
}