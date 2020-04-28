
#include <SD.h>
#include <TFT.h> // Arduino LCD library
#include <SPI.h>
// pin definition for the Uno
#define cs 10
#define dc 9
#define rst 8
#include "tetrimino.h"

// pin definition for the Leonardo
// #define cs   7
// #define dc   0
// #define rst  1

const int downButtonPin = A3;
const int leftButtonPin = A2;
const int rightButtonPin = A1;
const int upButtonPin = A0;

#define TFT_MAX_WIDTH 120
#define TFT_MAX_HEIGHT 160
#define CELL_SIZE 8

// Color definitions
/* some RGB color definitions                                                 */
#define Black 0x0000 /*   0,   0,   0 */
// #define DarkRed            0x000F      /*   0,   0, 128 */
// #define Green       0x03E0      /*   0, 128,   0 */
#define Yellow 0x03EF /*   0, 128, 128 */
// #define DarkBlue          0x7800      /* 128,   0,   0 */
#define Pink 0x780F      /* 128,   0, 128 */
#define Olive 0x7BE0     /* 128, 128,   0 */
#define LightGrey 0xC618 /* 192, 192, 192 */
#define DarkGrey 0x7BEF  /* 128, 128, 128 */
#define Red 0x001F       /*   0,   0, 255 */
#define Green 0x07E0     /*   0, 255,   0 */
#define Todo 0x07FF      /*   0, 255, 255 */
// #define DarkBlue             0xF800      /* 255,   0,   0 */
// #define Magenta         0xF81F      /* 255,   0, 255 */
#define Blue 0xFFE0  /* 255, 255,   0 */
#define White 0xFFFF /* 255, 255, 255 */
// #define Blue          0xFD20      /* 255, 165,   0 */
#define GreenYellow 0xAFE5 /* 173, 255,  47 */

// Playfield definition
#define CELLS_WIDE 10
#define CELLS_TALL 20
#define HIDDEN_CELLS 2
#define CELL_SIZE 8
#define PLAYFIELD_X_OFFSET 0
#define VOID_CELL BLACK

int multiplier = 8;

// create an instance of the library
TFT TFTscreen = TFT(cs, dc, rst);

//0°) i=wy + x
//90°) i=w(w-1) + y - wx
//180°) i= w^2 - 1 - wy - x
//270°) i= w - 1 - y +wx

// variables will change:
int downButtonState = 0;  // variable for reading the pushbutton status
int leftButtonState = 0;  // variable for reading the pushbutton status
int rightButtonState = 0; // variable for reading the pushbutton status
int upButtonState = 0;    // variable for reading the pushbutton status

// initial position of the point is the middle of the screen
// initial position of the point is the middle of the screen
int verticalDotCentrePosition = 80;
int horizontalDotCentrePosition = 64;

int level = 1;

// direction and speed
int speed = 1;

const int numInvisableRows = 2;
const int verticalPlayFieldStart = -1;
const int horizontalPlayFieldStart = 1;
const int tetrisGridCols = 10;
const int tetrisGridRows = 20;
const int tetrisGridRowsIncInvis = 20 + numInvisableRows;
const int heightPlayField = (tetrisGridRows * multiplier) + 2;
const int widthPlayField = (tetrisGridCols * multiplier) + 2;

const int linePosHorizontalMaxRight = horizontalPlayFieldStart + widthPlayField;
const int linePosVerticalMaxUp = verticalPlayFieldStart + 1;
const int linePosVerticalMaxDown = verticalPlayFieldStart + heightPlayField - 1;
const int linePosHorizontalMaxLeft = horizontalPlayFieldStart;

const int verticalStartPosition = linePosVerticalMaxUp - (numInvisableRows * multiplier);

const int horizontalStartPosition = linePosHorizontalMaxLeft + (widthPlayField / 2) - multiplier;

// variables to keep track of the point's location
int verticalDotPosition = verticalStartPosition;
int horizontalDotPosition = horizontalStartPosition;
int previousVerticalDotPosition = verticalStartPosition;
int previousHorizontalDotPosition = horizontalStartPosition;
int previousLevel = level;
long timeTaken;
long previousTime;
long fallSpeed = 1000;
boolean alive;

Tetrimino o;
Tetrimino l;
Tetrimino t;

uint16_t playGrid[tetrisGridCols * tetrisGridRowsIncInvis];

void setup()
{
  Serial.begin(9600);
  Serial.println("start");

  // Put this line at the beginning of every sketch that uses the GLCD:
  TFTscreen.begin();
  TFTscreen.setRotation(2);

  // TFTscreen.invertDisplay(0);
  // clear the screen with a black background
  TFTscreen.background(Black);
  int counter;
  for (int counter = 0; counter < tetrisGridCols * tetrisGridRowsIncInvis; counter++)
  {
    playGrid[counter] = 0;
  }

  const int oRows = 2;
  const int oCols = 2;
  int oArray[oRows * oCols] = {
      1,
      1,
      1,
      1,
  };

  o.initialise(oArray, oRows, oCols, Yellow, multiplier);

  const int lRows = 4;
  const int lCols = 4;
  int lArray[lRows * lCols] = {0, 0, 0, 0,
                               1, 1, 1, 1,
                               0, 0, 0, 0,
                               0, 0, 0, 0};

  l.initialise(lArray, lRows, lCols, Blue, multiplier);

  const int tRows = 3;
  const int tCols = 3;
  int tArray[tRows * tCols] = {0, 0, 0,
                               0, 1, 0,
                               1, 1, 1};

  t.initialise(tArray, tRows, tCols, Pink, multiplier);

  // initialize the pushbutton pin as an input:
  pinMode(downButtonPin, INPUT);
  pinMode(leftButtonPin, INPUT);
  pinMode(rightButtonPin, INPUT);
  pinMode(upButtonPin, INPUT);

  TFTscreen.stroke(White);
  //screen.rect(xStart, yStart, width, height);
  //xStart : int, the horizontal position where the line starts
  //yStart : int, the vertical position where the line starts
  //width : int, the width of the rectangle
  //height : int, the height of the rectangle
  TFTscreen.rect(horizontalPlayFieldStart, verticalPlayFieldStart, widthPlayField, heightPlayField);
}

void loop()
{
  Serial.println("o");
  spawnTetrimino(o);
  Serial.println("l");
  spawnTetrimino(l);
  Serial.println("t");
  spawnTetrimino(t);
  // int e, p;
  // delay(5000);
// 
  // for (int e = 0; e < tetrisGridRowsIncInvis; e++)
  // {
  //   Serial.println("");
  //   for (int p = 0; p < tetrisGridCols; p++)
  //   {
// 
  //     if (playGrid[p + tetrisGridCols * e] == 0)
  //     {
  //       Serial.print("o");
  //     }
  //     else
  //     {
  //       Serial.print("x");
  //     }
  //   }
  // }
  // delay(10000);
}

void spawnTetrimino(Tetrimino tetrimino)
{

  alive = true;
  TFTscreen.stroke(tetrimino.colour);
  verticalDotPosition = verticalStartPosition;
  previousVerticalDotPosition = verticalStartPosition;

  horizontalDotPosition = horizontalStartPosition;
  previousHorizontalDotPosition = horizontalDotPosition;

  previousTime = millis();
  while (alive)
  {
    moveTetrimino(tetrimino);
    timeTaken = millis() - previousTime;
    if (timeTaken >= fallSpeed)
    {
      tryToMoveDown(tetrimino);
      previousTime = millis();
    };
  }

  //No longer alive... now we need to commit the block to the playing field.

  commitToPlayGrid(tetrimino);
}

void commitToPlayGrid(Tetrimino tetrimino)
{

  for (int n = 0; n < tetrimino.cols; n++)
  {
    for (int m = 0; m < tetrimino.rows; m++)
    {

      if (tetrimino.booleanOfGrid(m, n))
      {
        // Serial.print(m);
        // Serial.print(",");
        // Serial.print(n);
        // Serial.print(" : playgrid[");
        // Serial.print((((horizontalDotPosition - linePosHorizontalMaxLeft) / multiplier) + m) + (tetrisGridCols * (numInvisableRows + ((verticalDotPosition - linePosVerticalMaxUp) / multiplier) + n)));
        // Serial.print("]");
        // Serial.print(" AKA  playgrid[");
        // Serial.print(((horizontalDotPosition - linePosHorizontalMaxLeft) / multiplier) + m);
        // Serial.print("][");
        // Serial.print((numInvisableRows + ((verticalDotPosition - linePosVerticalMaxUp) / multiplier) + n));
        // Serial.print("] ==     ");
        // Serial.println(tetrimino.colour);

        playGrid[(((horizontalDotPosition - linePosHorizontalMaxLeft) / multiplier) + m) + (tetrisGridCols * (numInvisableRows + ((verticalDotPosition - linePosVerticalMaxUp) / multiplier) + n))] = tetrimino.colour;
      }
    }
  }
}

void moveTetrimino(Tetrimino tetrimino)
{
  downButtonState = digitalRead(downButtonPin);
  upButtonState = digitalRead(upButtonPin);
  leftButtonState = digitalRead(leftButtonPin);
  rightButtonState = digitalRead(rightButtonPin);

  if (downButtonState == HIGH)
  {
    //TODO - implement legit faster fall rules
    tryToMoveDown(tetrimino);
  }
  if (upButtonState == HIGH)
  {
    //Cannot go up- implement hard drop...
  }
  if (leftButtonState == HIGH)
  {

    if (horizontalDotPosition - (speed * multiplier) <= linePosHorizontalMaxLeft)
    {
      Serial.println("MAX_LEFT");
    }
    else
    {

      movedLeft(tetrimino);
    }
  }
  if (rightButtonState == HIGH)
  {
    Serial.println(horizontalDotPosition);

    if (horizontalDotPosition + ((speed + tetrimino.blocksRight()) * multiplier) >= linePosHorizontalMaxRight)

    {
      Serial.println("MAX_RIGHT");
    }
    else
    {

      movedRight(tetrimino);
    }
  }
  previousLevel = level;
  delay(50);
}

void tryToMoveDown(Tetrimino tetrimino)
{
//if the block settles above the visable line, end the game.

  int hypotheticalVerticalDotPosition = verticalDotPosition + (speed  * multiplier);
  if (hitBottom(tetrimino, hypotheticalVerticalDotPosition))
  {
    // TODO combine into one or statement when it works
    alive = false;
  }
  else if (overlapOfPlayGrid(tetrimino, hypotheticalVerticalDotPosition))
  {
    alive = false;
  }
  else
  {
verticalDotPosition = hypotheticalVerticalDotPosition;
    movedDown(tetrimino);
  }
}

boolean hitBottom(Tetrimino tetrimino, int hypotheticalVerticalDotPosition)
{
  if (hypotheticalVerticalDotPosition + (tetrimino.blocksDown() * multiplier) > linePosVerticalMaxDown)
  {
    return true;
  }
  return false;
}

boolean overlapOfPlayGrid(Tetrimino tetrimino, int hypotheticalVerticalDotPosition)
{
  for (int n = 0; n < tetrimino.cols; n++)
  {
    for (int m = 0; m < tetrimino.rows; m++)
    {

      if (tetrimino.booleanOfGrid(m, n))
      {
        if (playGrid[(((horizontalDotPosition - linePosHorizontalMaxLeft) / multiplier) + m) + (tetrisGridCols * (numInvisableRows + ((hypotheticalVerticalDotPosition - linePosVerticalMaxUp) / multiplier) + n))] != 0)
        {
          return true;
        }
      }
    }
  }
  return false;
}

double calculateFallSpeed(int level)
{
  return pow(0.8 - ((level - 1) * 0.007), (level - 1));
}

void movedRight(Tetrimino tetrimino)
{
  horizontalDotPosition = horizontalDotPosition + (speed * multiplier);

  moveScreenTetrimino(tetrimino);

  previousHorizontalDotPosition = horizontalDotPosition;
}

void movedLeft(Tetrimino tetrimino)
{
  horizontalDotPosition = horizontalDotPosition - (speed * multiplier);

  moveScreenTetrimino(tetrimino);

  previousHorizontalDotPosition = horizontalDotPosition;
}

void movedDown(Tetrimino tetrimino)
{
  moveScreenTetrimino(tetrimino);
  previousVerticalDotPosition = verticalDotPosition;
}

void moveScreenTetrimino(Tetrimino tetrimino)
{
  erasePreviousColouredPoints(tetrimino);
  createNewColouredPoints(tetrimino);
}

void erasePreviousColouredPoints(Tetrimino tetrimino)
{
  fillInOldGrid(tetrimino, true, Black);
}

void createNewColouredPoints(Tetrimino tetrimino)
{
  fillInNewGrid(tetrimino, true, tetrimino.colour);
}

void fillInNewGrid(Tetrimino tetrimino, boolean trueOrFalse, uint16_t colour)
{
  fillInGrid(tetrimino, horizontalDotPosition, verticalDotPosition, trueOrFalse, colour);
}

void fillInOldGrid(Tetrimino tetrimino, boolean trueOrFalse, uint16_t colour)
{
  fillInGrid(tetrimino, previousHorizontalDotPosition, previousVerticalDotPosition, trueOrFalse, colour);
}

void fillInGrid(Tetrimino tetrimino, int startHorizontalDotPosition, int startVerticalDotPosition, boolean trueOrFalse, uint16_t colour)
{
  for (int m = 0; m < tetrimino.rows; m++)
  {
    for (int n = 0; n < tetrimino.cols; n++)
    {

      if (tetrimino.booleanOfGrid(m, n) == trueOrFalse)
      {

        TFTscreen.fillRect(startHorizontalDotPosition + (multiplier * m), startVerticalDotPosition + (multiplier * n), multiplier, multiplier, colour);
      }
    }
  }
}