
// #include <SD.h>
#include <TFT.h> // Arduino LCD library
#include <SPI.h>
#include <Arduino.h>
// pin definition for the Uno
// #define cs 10
// #define dc 9
// #define rst 8
#include "tetrimino.h"

/*
 STM32 SPI1/SPI2 pins:
 
 SPI1 MOSI PA7
 SPI1 MISO PA6
 SPI1 SCK  PA5
 SPI1 CS   PA4
 SPI2 MOSI PB15
 SPI2 MISO PB14
 SPI2 SCK  PB13
 SPI2 CS   PB12
*/

#define TFT_CS PB10
#define TFT_DC PB0
#define TFT_RST PB1

#define DOWN_BUTTON PA4
#define UP_BUTTON PA2
#define RIGHT_BUTTON PA6
#define LEFT_BUTTON PA3

#define B_BUTTON PA1
#define A_BUTTON PA0

#define TFT_MAX_WIDTH 120
#define TFT_MAX_HEIGHT 160
#define CELL_SIZE 8

// Color definitions
/* some RGB color definitions                                                 */
#define Black 0x0000
// #define DarkRed            0x000F
#define Green 0x03E0
#define Yellow 0x03EF
#define DarkBlue 0x7800
#define Pink 0x780F
#define Olive 0x7BE0
#define LightGrey 0xC618
#define DarkGrey 0x7BEF
#define Red 0x001F
#define Orange 0x02EF
#define Green 0x07E0
// #define DarkBlue             0xF800      /* 255,   0,   0 */
// #define Magenta         0xF81F      /* 255,   0, 255 */
#define Blue 0xFFE0
#define White 0xFFFF
#define Blue 0xFD20
#define GreenYellow 0xAFE5

int multiplier = 8;

// create an instance of the library
TFT TFTscreen = TFT(TFT_CS, TFT_DC, TFT_RST);

// variables will change:
int downButtonState = 0;  // variable for reading the pushbutton status
int leftButtonState = 0;  // variable for reading the pushbutton status
int rightButtonState = 0; // variable for reading the pushbutton status
int upButtonState = 0;    // variable for reading the pushbutton status
int bButtonState = 0;     // variable for reading the pushbutton status
int aButtonState = 0;     // variable for reading the pushbutton status

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

const int middleOfPlayField = linePosHorizontalMaxLeft + (widthPlayField / 2);
const int horizontalStartPosition = middleOfPlayField - multiplier;

// variables to keep track of the point's location
int verticalDotPosition = verticalStartPosition;
int horizontalDotPosition = horizontalStartPosition;
int previousVerticalDotPosition = verticalStartPosition;
int previousHorizontalDotPosition = horizontalStartPosition;

int verticalGhostPosition = verticalStartPosition;
int previousVerticalGhostPosition = verticalStartPosition;

int lineMultiplier;

int previousLevel = level;
long timeTaken;
long previousTime;
long fallSpeed = 1000;
boolean tetriminoAlive;
boolean gameAlive;
boolean letGoOfHardDrop;
boolean letGoOfRotate;
int *currentTetriminoGrid;

Tetrimino o;
Tetrimino l;
Tetrimino t;
Tetrimino i;
Tetrimino j;
Tetrimino s;
Tetrimino z;

    Tetrimino *tetriminoArray[7] = {&o,&l,&t,&i,&j,&s,&z };

uint16_t playGrid[tetrisGridCols * tetrisGridRowsIncInvis];

void spawnTetrimino(Tetrimino tetrimino);

void moveTetrimino(Tetrimino tetrimino);
void fillInGrid(Tetrimino tetrimino, int startHorizontalDotPosition, int startVerticalDotPosition, boolean trueOrFalse, uint16_t colour);
void ghostGrid(Tetrimino tetrimino, int startHorizontalDotPosition, int startVerticalDotPosition, boolean trueOrFalse, uint16_t colour);

void fillInOldGrid(Tetrimino tetrimino, boolean trueOrFalse, uint16_t strokeColour);
void fillInNewGrid(Tetrimino tetrimino, boolean trueOrFalse, uint16_t strokeColour);
void createNewColouredPoints(Tetrimino tetrimino);
void erasePreviousColouredPoints(Tetrimino tetrimino);
void moveScreenTetrimino(Tetrimino tetrimino);
void rotateScreenTetrimino(Tetrimino tetrimino);
void movedDown(Tetrimino tetrimino);
double calculateFallSpeed(int level);
void movedRight(Tetrimino tetrimino);
void movedLeft(Tetrimino tetrimino);
void tryToRotate(Tetrimino tetrimino);
void tryToMoveDown(Tetrimino tetrimino);
void tryToMoveRight(Tetrimino tetrimino);
void tryToMoveLeft(Tetrimino tetrimino);
void ghostPositionUpdate(Tetrimino tetrimino);
boolean hitBottom(Tetrimino tetrimino, int hypotheticalVerticalDotPosition);
boolean overlapOfPlayGrid(Tetrimino tetrimino, int hypotheticalHorizontalDotPosition, int hypotheticalVerticalDotPosition);
void commitToPlayGrid(Tetrimino tetrimino);
void clearGrid();
void hardDrop(Tetrimino tetrimino);
void spawnGhost(Tetrimino tetrimino);
void ghostColour(Tetrimino tetrimino);
void updateGhost(Tetrimino tetrimino);
void updateGhostColour(Tetrimino tetrimino);
void checkForClearLine();
void randomSeven();
int selectorRand[7];

void setup()
{
  SerialUSB.begin();
  SerialUSB.println("start");

  // Put this line at the beginning of every sketch that uses the GLCD:
  TFTscreen.begin();
  TFTscreen.setRotation(0);

  const int oRows = 2;
  const int oCols = 2;
  int oArray[oRows * oCols] = {
      1,
      1,
      1,
      1,
  };

  o.initialise(oArray, oRows, oCols, Yellow, multiplier);

  const int iRows = 4;
  const int iCols = 4;
  int iArray[iRows * iCols] = {0, 0, 0, 0,
                               1, 1, 1, 1,
                               0, 0, 0, 0,
                               0, 0, 0, 0};

  i.initialise(iArray, iRows, iCols, Blue, multiplier);

  const int tRows = 3;
  const int tCols = 3;
  int tArray[tRows * tCols] = {0, 1, 0,
                               1, 1, 1,
                               0, 0, 0};

  t.initialise(tArray, tRows, tCols, Pink, multiplier);

  const int lRows = 3;
  const int lCols = 3;
  int lArray[lRows * lCols] = {0, 0, 1,
                               1, 1, 1,
                               0, 0, 0};

  l.initialise(lArray, lRows, lCols, Orange, multiplier);

  const int jRows = 3;
  const int jCols = 3;
  int jArray[jRows * jCols] = {1, 0, 0,
                               1, 1, 1,
                               0, 0, 0};

  j.initialise(jArray, jRows, jCols, DarkBlue, multiplier);

  const int sRows = 3;
  const int sCols = 3;
  int sArray[sRows * sCols] = {0, 1, 1,
                               1, 1, 0,
                               0, 0, 0};

  s.initialise(sArray, sRows, sCols, Green, multiplier);

  const int zRows = 3;
  const int zCols = 3;
  int zArray[zRows * zCols] = {1, 1, 0,
                               0, 1, 1,
                               0, 0, 0};

  z.initialise(zArray, zRows, zCols, Red, multiplier);




  pinMode(DOWN_BUTTON, INPUT);
  pinMode(UP_BUTTON, INPUT);
  pinMode(LEFT_BUTTON, INPUT);
  pinMode(RIGHT_BUTTON, INPUT);
  pinMode(B_BUTTON, INPUT);
  pinMode(A_BUTTON, INPUT);

  // clear the screen with a black background
  TFTscreen.background(Black);
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

  TFTscreen.fillRect(linePosHorizontalMaxLeft + 1, linePosVerticalMaxUp, widthPlayField - 2, heightPlayField, Black);
  gameAlive = true;
  clearGrid();

randomSeven();
int randomTetriminos[14];
  for (int index = 0; index < 7; index++) {
randomTetriminos[index] = selectorRand[index];
  }

  randomSeven();
    for (int index = 0; index < 7; index++) {
randomTetriminos[index+7] = selectorRand[index];
  }

  while (gameAlive)
  {

   for (int index = 0; index < 7; index++) {
    spawnTetrimino( *tetriminoArray[randomTetriminos[index]]);
    checkForClearLine();
  }
  for (int index = 0; index < 7; index++) {
randomTetriminos[index] = randomTetriminos[index+7];

  }
  randomSeven();
    for (int index = 0; index < 7; index++) {
randomTetriminos[index+7] = selectorRand[index];
  }

  }
  TFTscreen.fillRect(linePosHorizontalMaxLeft + 1, linePosVerticalMaxUp, widthPlayField - 2, heightPlayField, Black);
  TFTscreen.stroke(White);
  TFTscreen.text("GAME OVER", (middleOfPlayField - strlen("GAME OVER")) / 2, verticalDotCentrePosition);
  delay(5000);

}

void randomSeven(){
      int selector[7] = {0,1,2,3,4,5,6 };
  for (int index = 0; index < 7; index++) {
    int randomInt = (rand() % (7-index));
    selectorRand[index] = selector[randomInt];
for (int subIndex = randomInt; subIndex < 6; subIndex++) {
selector[subIndex] = selector[subIndex+1];
  }
  }

}

void clearGrid()
{
  for (int counter = 0; counter < tetrisGridCols * tetrisGridRowsIncInvis; counter++)
  {
    playGrid[counter] = 0;
  }
}

void checkForClearLine()
{
  lineMultiplier = 0;

  boolean needsReDoing = false;
  for (int m = 0; m < tetrisGridRowsIncInvis; m++)
  {
    for (int n = 0; n < tetrisGridCols; n++)
    {

      if (playGrid[n + (tetrisGridCols * m)] != 0)
      {
        if (n == tetrisGridCols - 1)
        {
          SerialUSB.print(m);
          SerialUSB.println("   < WOOOOOOOOOOOO LINE");
          lineMultiplier++;
          needsReDoing = true;

          // Shift above lines...
          for (int u = m; u > 0; u--)
          {
            for (int r = 0; r < tetrisGridCols; r++)
            {
              playGrid[r + (tetrisGridCols * u)] = playGrid[r + (tetrisGridCols * (u-1))];
            }
          }

          //New blank line
          for (int r = 0; r < tetrisGridCols; r++)
          {
            playGrid[ r] = 0;
          }
        }
      }
      else
      {
        n = tetrisGridCols;
      }
    }
  }

  if (needsReDoing)
  {
    TFTscreen.fillRect(linePosHorizontalMaxLeft + 1, linePosVerticalMaxUp, widthPlayField - 2, heightPlayField, Black);
 

    for (int u = 0; u < tetrisGridRowsIncInvis; u++)
    {
      for (int r = 0; r < tetrisGridCols; r++)
      {

        uint16_t colourOfGrid = playGrid[r + (tetrisGridCols * u)];
        if (colourOfGrid != 0)
        {
          int rationalHorizontal = ((r* multiplier) + linePosHorizontalMaxLeft + 2) ;

          int rationalVertical = ((u - numInvisableRows) * multiplier) + linePosVerticalMaxUp+1;

          SerialUSB.print(rationalHorizontal);
          SerialUSB.print(" , ");
          SerialUSB.println(rationalVertical);

          TFTscreen.fillRect(rationalHorizontal, rationalVertical, multiplier - 1, multiplier - 1, colourOfGrid);
        }
        //Do something for the points here...
      }
    }
  }
}

void spawnTetrimino(Tetrimino tetrimino)
{
  tetrimino.spawn();
  tetriminoAlive = true;
  TFTscreen.stroke(tetrimino.colour);
  verticalDotPosition = verticalStartPosition;
  previousVerticalDotPosition = verticalStartPosition;
  verticalGhostPosition = verticalDotPosition;
  previousVerticalGhostPosition = verticalDotPosition;

  horizontalDotPosition = horizontalStartPosition;
  previousHorizontalDotPosition = horizontalDotPosition;

  spawnGhost(tetrimino);
  letGoOfHardDrop = false;
  previousTime = millis();
  while (tetriminoAlive)
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
  if (verticalDotPosition < linePosVerticalMaxUp)
  {
    gameAlive = false;
  }
}

void commitToPlayGrid(Tetrimino tetrimino)
{

  for (int m = 0; m < tetrimino.rows; m++)
  {

    for (int n = 0; n < tetrimino.cols; n++)
    {

      if (tetrimino.booleanOfGrid(m, n))
      {
        int rationalHorizontal = (horizontalDotPosition - linePosHorizontalMaxLeft - 1) / multiplier;
        int rationalVertical = numInvisableRows + ((verticalDotPosition - linePosVerticalMaxUp) / multiplier);
        playGrid[(rationalHorizontal + m) + (tetrisGridCols * (rationalVertical + n))] = tetrimino.colour;
        //  SerialUSB.print(m);
        //   SerialUSB.print(" , ");
        //   SerialUSB.print(n);
        //   SerialUSB.print(" : ");
        //   SerialUSB.print(rationalHorizontal);
        //     SerialUSB.print(" (");

        //   SerialUSB.print(horizontalDotPosition);
        //       SerialUSB.print(")");
        //   SerialUSB.print(" , ");
        //   SerialUSB.print(rationalVertical);
        //    SerialUSB.print(" > ");
        //   SerialUSB.println((rationalHorizontal + m) + (tetrisGridCols * (rationalVertical + n)));
      }
    }
  }
  //  SerialUSB.println("*************************************");
}

void moveTetrimino(Tetrimino tetrimino)
{
  downButtonState = digitalRead(DOWN_BUTTON);
  upButtonState = digitalRead(UP_BUTTON);
  leftButtonState = digitalRead(LEFT_BUTTON);
  rightButtonState = digitalRead(RIGHT_BUTTON);
  bButtonState = digitalRead(B_BUTTON);
  aButtonState = digitalRead(A_BUTTON);
  if (upButtonState == LOW)
  {
    letGoOfHardDrop = true;
  }

  if (downButtonState == HIGH)
  {
    //TODO - implement legit faster fall rules
    tryToMoveDown(tetrimino);
  }
  if (upButtonState == HIGH && letGoOfHardDrop)
  {
    hardDrop(tetrimino);
  }
  if (leftButtonState == HIGH)
  {
    tryToMoveLeft(tetrimino);
  }
  if (rightButtonState == HIGH)
  {

    tryToMoveRight(tetrimino);
  }

  if (aButtonState == HIGH)
  {
    // HOLD
  }
  if (bButtonState == HIGH && letGoOfRotate)
  {

    letGoOfRotate = false;
    tryToRotate(tetrimino);
  }
  if (bButtonState == LOW)
  {
    letGoOfRotate = true;
  }

  previousLevel = level;
  delay(50);
}

void hardDrop(Tetrimino tetrimino)
{
  int hypotheticalVerticalDotPosition = verticalDotPosition + (speed * multiplier);

  while (!hitBottom(tetrimino, hypotheticalVerticalDotPosition) && !(overlapOfPlayGrid(tetrimino, horizontalDotPosition, hypotheticalVerticalDotPosition)))
  {
    verticalDotPosition = hypotheticalVerticalDotPosition;
    hypotheticalVerticalDotPosition = verticalDotPosition + (speed * multiplier);
  }
  movedDown(tetrimino);
  tetriminoAlive = false;
}

void ghostPositionUpdate(Tetrimino tetrimino)
{
  verticalGhostPosition = verticalDotPosition;
  int hypotheticalVerticalGhostPosition = verticalGhostPosition + (speed * multiplier);

  while (!hitBottom(tetrimino, hypotheticalVerticalGhostPosition) && !(overlapOfPlayGrid(tetrimino, horizontalDotPosition, hypotheticalVerticalGhostPosition)))
  {
    verticalGhostPosition = hypotheticalVerticalGhostPosition;
    hypotheticalVerticalGhostPosition = verticalGhostPosition + (speed * multiplier);
  }
}

void updateGhost(Tetrimino tetrimino)
{
  ghostPositionUpdate(tetrimino);

  updateGhostColour(tetrimino);
}

void spawnGhost(Tetrimino tetrimino)
{
  verticalGhostPosition = verticalDotPosition;
  int hypotheticalVerticalGhostPosition = verticalGhostPosition + (speed * multiplier);

  while (!hitBottom(tetrimino, hypotheticalVerticalGhostPosition) && !(overlapOfPlayGrid(tetrimino, horizontalDotPosition, hypotheticalVerticalGhostPosition)))
  {
    verticalGhostPosition = hypotheticalVerticalGhostPosition;
    hypotheticalVerticalGhostPosition = verticalGhostPosition + (speed * multiplier);
  }
  ghostColour(tetrimino);
}

void tryToMoveDown(Tetrimino tetrimino)
{
  //if the block settles above the visable line, end the game.

  int hypotheticalVerticalDotPosition = verticalDotPosition + (speed * multiplier);
  if (hitBottom(tetrimino, hypotheticalVerticalDotPosition))
  {
    // TODO combine into one or statement when it works
    tetriminoAlive = false;
  }
  else if (overlapOfPlayGrid(tetrimino, horizontalDotPosition, hypotheticalVerticalDotPosition))
  {
    tetriminoAlive = false;
  }
  else
  {
    verticalDotPosition = hypotheticalVerticalDotPosition;
    movedDown(tetrimino);
  }
}

void tryToRotate(Tetrimino tetrimino)
{

  tetrimino.rotateGrid();
  if (horizontalDotPosition + (tetrimino.blocksRight() * multiplier) >= linePosHorizontalMaxRight || horizontalDotPosition + (tetrimino.blocksLeft() * multiplier) <= linePosHorizontalMaxLeft)

  {
    tetrimino.unrotateGrid();
  }
  else if (overlapOfPlayGrid(tetrimino, horizontalDotPosition, verticalDotPosition))
  {
    tetrimino.unrotateGrid();
  }
  else
  {
    tetrimino.unrotateGrid();
    rotateScreenTetrimino(tetrimino);
  }
}

void tryToMoveRight(Tetrimino tetrimino)
{
  int hypotheticalHorizontalDotPosition = horizontalDotPosition + (speed * multiplier);

  if (hypotheticalHorizontalDotPosition + (tetrimino.blocksRight() * multiplier) >= linePosHorizontalMaxRight)

  {
  }
  else if (overlapOfPlayGrid(tetrimino, hypotheticalHorizontalDotPosition, verticalDotPosition))
  {
  }
  else
  {
    horizontalDotPosition = hypotheticalHorizontalDotPosition;
    movedRight(tetrimino);
  }
}

void tryToMoveLeft(Tetrimino tetrimino)
{
  int hypotheticalHorizontalDotPosition = horizontalDotPosition - (speed * multiplier);

  if (hypotheticalHorizontalDotPosition + (tetrimino.blocksLeft() * multiplier) <= linePosHorizontalMaxLeft)

  {
  }
  else if (overlapOfPlayGrid(tetrimino, hypotheticalHorizontalDotPosition, verticalDotPosition))
  {
  }
  else
  {
    horizontalDotPosition = hypotheticalHorizontalDotPosition;
    movedLeft(tetrimino);
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

boolean overlapOfPlayGrid(Tetrimino tetrimino, int hypotheticalHorizontalDotPosition, int hypotheticalVerticalDotPosition)
{
  for (int n = 0; n < tetrimino.cols; n++)
  {
    for (int m = 0; m < tetrimino.rows; m++)
    {

      if (tetrimino.booleanOfGrid(m, n))
      {
        if (playGrid[(((hypotheticalHorizontalDotPosition - linePosHorizontalMaxLeft - 1) / multiplier) + m) + (tetrisGridCols * (numInvisableRows + ((hypotheticalVerticalDotPosition - linePosVerticalMaxUp) / multiplier) + n))] != 0)
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
  updateGhost(tetrimino);
  moveScreenTetrimino(tetrimino);

  previousHorizontalDotPosition = horizontalDotPosition;
}

void movedLeft(Tetrimino tetrimino)
{
  updateGhost(tetrimino);
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

void updateGhostColour(Tetrimino tetrimino)
{
  ghostGrid(tetrimino, previousHorizontalDotPosition, previousVerticalGhostPosition, true, Black);
  ghostColour(tetrimino);
}

void ghostColour(Tetrimino tetrimino)
{
  ghostGrid(tetrimino, horizontalDotPosition, verticalGhostPosition, true, tetrimino.colour);
  previousVerticalGhostPosition = verticalGhostPosition;
}

void rotateScreenTetrimino(Tetrimino tetrimino)
{
  ghostGrid(tetrimino, horizontalDotPosition, verticalGhostPosition, true, Black);
  fillInGrid(tetrimino, horizontalDotPosition, verticalDotPosition, true, Black);
  tetrimino.rotateGrid();
  ghostPositionUpdate(tetrimino);
  ghostColour(tetrimino);
  fillInGrid(tetrimino, horizontalDotPosition, verticalDotPosition, true, tetrimino.colour);
}

void erasePreviousColouredPoints(Tetrimino tetrimino)
{
  fillInOldGrid(tetrimino, true, Black);
}

void createNewColouredPoints(Tetrimino tetrimino)
{
  fillInNewGrid(tetrimino, true, tetrimino.colour);
}

void fillInNewGrid(Tetrimino tetrimino, boolean trueOrFalse, uint16_t strokeColour)
{
  fillInGrid(tetrimino, horizontalDotPosition, verticalDotPosition, trueOrFalse, strokeColour);
}

void fillInOldGrid(Tetrimino tetrimino, boolean trueOrFalse, uint16_t strokeColour)
{
  fillInGrid(tetrimino, previousHorizontalDotPosition, previousVerticalDotPosition, trueOrFalse, strokeColour);
}

void fillInGrid(Tetrimino tetrimino, int startHorizontalDotPosition, int startVerticalDotPosition, boolean trueOrFalse, uint16_t colour)
{
  for (int m = 0; m < tetrimino.rows; m++)
  {
    for (int n = 0; n < tetrimino.cols; n++)
    {

      if (tetrimino.booleanOfGrid(m, n) == trueOrFalse)
      {

        TFTscreen.fillRect(startHorizontalDotPosition + 1 + (multiplier * m), startVerticalDotPosition + 1 + (multiplier * n), multiplier - 1, multiplier - 1, colour);
      }
    }
  }
}

void ghostGrid(Tetrimino tetrimino, int startHorizontalDotPosition, int startVerticalDotPosition, boolean trueOrFalse, uint16_t strokeColour)
{

  for (int m = 0; m < tetrimino.rows; m++)
  {
    for (int n = 0; n < tetrimino.cols; n++)
    {

      if (tetrimino.booleanOfGrid(m, n) == trueOrFalse)
      {
        TFTscreen.stroke(strokeColour);

        TFTscreen.rect(startHorizontalDotPosition + 1 + (multiplier * m), startVerticalDotPosition + 1 + (multiplier * n), multiplier - 1, multiplier - 1);
      }
    }
  }
}
