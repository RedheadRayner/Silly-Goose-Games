
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

int multiplier;

// create an instance of the library
TFT TFTscreen = TFT(TFT_CS, TFT_DC, TFT_RST);

// variables will change:
int downButtonState = 0;  // variable for reading the pushbutton status
int leftButtonState = 0;  // variable for reading the pushbutton status
int rightButtonState = 0; // variable for reading the pushbutton status
int upButtonState = 0;    // variable for reading the pushbutton status
int bButtonState = 0;     // variable for reading the pushbutton status
int aButtonState = 0;     // variable for reading the pushbutton status

int16_t screenShort = TFTscreen.height();
int16_t screenLong = TFTscreen.width();

// initial position of the point is the middle of the screen
// initial position of the point is the middle of the screen
int verticalDotCentrePosition;
int horizontalDotCentrePosition;

int level = 1;

// direction and speed
int speed = 1;

// const int lineDrawBuffer = 1;
// const int numInvisableRows = 2;
// const int verticalPlayFieldStart = -1;
// const int horizontalPlayFieldStart = 1;
// const int tetrisGridCols = 10;
// const int tetrisGridRows = 20;
// const int tetrisGridRowsIncInvis = 20 + numInvisableRows;
// const int heightPlayField = (tetrisGridRows * multiplier) + 2;
// const int widthPlayField = (tetrisGridCols * multiplier) + 2;

// const int linePosHorizontalMaxRight = horizontalPlayFieldStart + widthPlayField;
// const int linePosVerticalMaxUp = verticalPlayFieldStart + 1;
// const int linePosVerticalMaxDown = verticalPlayFieldStart + heightPlayField - 1;
// const int linePosHorizontalMaxLeft = horizontalPlayFieldStart;

// const int verticalStartPosition = linePosVerticalMaxUp - (numInvisableRows * multiplier);

// const int middleOfPlayField = linePosHorizontalMaxLeft + (widthPlayField / 2);
// const int horizontalStartPosition = middleOfPlayField - multiplier;

const int lineDrawBuffer = 1;
const int numInvisableRows = 2;
const int tetrisGridCols = 10;
const int tetrisGridRows = 20;
const int tetrisGridRowsIncInvis = tetrisGridRows + numInvisableRows;

int verticalPlayFieldStart;
int horizontalPlayFieldStart;

int heightPlayFieldContainer;
int widthPlayFieldContainer;

int heightPlayField;
int widthPlayField;

int linePosHorizontalMaxRight;
int linePosVerticalMaxUp;
int linePosVerticalMaxDown;
int linePosHorizontalMaxLeft;

int verticalStartPosition;

int middleOfPlayField;
int horizontalStartPosition;

// variables to keep track of the point's location
int verticalDotPosition;
int horizontalDotPosition;
int previousVerticalDotPosition;
int previousHorizontalDotPosition;

int verticalGhostPosition;
int previousVerticalGhostPosition;

int lineMultiplier;

int previousLevel = level;
long timeTaken;
long previousTime;
long fallSpeed = 1000;
boolean tetriminoAlive;
boolean tetriminoInPlay;
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

Tetrimino holdTetrimino;

Tetrimino *tetriminoArray[7] = {&o, &l, &t, &i, &j, &s, &z};

uint16_t playGrid[tetrisGridCols * tetrisGridRowsIncInvis];

int sideBoxUnit;
int holdBoxVertical;

int nextUpVertical;
int sideBoxHorizontal;

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
void placeTetriminoInHold(Tetrimino tetrimino);
void clearGrid();
void hardDrop(Tetrimino tetrimino);
void spawnGhost(Tetrimino tetrimino);
void ghostColour(Tetrimino tetrimino);
void updateGhost(Tetrimino tetrimino);
void updateGhostColour(Tetrimino tetrimino);
void checkForClearLine();
void randomSeven();
boolean emptyHold;
boolean holdAvailable;
int selectorRand[7];

void setup()
{
  SerialUSB.begin();

  // Put this line at the beginning of every sketch that uses the GLCD:
  TFTscreen.begin();
  TFTscreen.setRotation(0);
  multiplier = 1;
  while (((float)screenLong / ((float)(tetrisGridRows * (multiplier + 1)))) > 1.0)
  {
    multiplier++;
  }
  heightPlayField = (tetrisGridRows * multiplier);
  heightPlayFieldContainer = heightPlayField + (2 * lineDrawBuffer);
  widthPlayField = (tetrisGridCols * multiplier);
  widthPlayFieldContainer = widthPlayField + (2 * lineDrawBuffer);

  verticalDotCentrePosition = screenLong / 2;
  horizontalDotCentrePosition = screenShort / 2;

  verticalPlayFieldStart = verticalDotCentrePosition - (heightPlayFieldContainer / 2);
  horizontalPlayFieldStart = verticalPlayFieldStart;

  linePosVerticalMaxUp = verticalPlayFieldStart + lineDrawBuffer;
  linePosVerticalMaxDown = linePosVerticalMaxUp + heightPlayField;
  linePosHorizontalMaxLeft = horizontalPlayFieldStart + lineDrawBuffer;
  linePosHorizontalMaxRight = linePosHorizontalMaxLeft + widthPlayField;

  verticalStartPosition = linePosVerticalMaxUp - (numInvisableRows * multiplier);

  middleOfPlayField = linePosHorizontalMaxLeft + (widthPlayField / 2);
  horizontalStartPosition = middleOfPlayField - multiplier;

  // variables to keep track of the point's location
  verticalDotPosition = verticalStartPosition;
  horizontalDotPosition = horizontalStartPosition;
  previousVerticalDotPosition = verticalStartPosition;
  previousHorizontalDotPosition = horizontalStartPosition;

  verticalGhostPosition = verticalStartPosition;
  previousVerticalGhostPosition = verticalStartPosition;

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
  TFTscreen.rect(horizontalPlayFieldStart, verticalPlayFieldStart, widthPlayFieldContainer, heightPlayFieldContainer);

  sideBoxUnit = (4 * multiplier) + 2 * lineDrawBuffer;
  sideBoxHorizontal = ((screenShort + (widthPlayFieldContainer + horizontalPlayFieldStart) - sideBoxUnit) / 2) - lineDrawBuffer;
  holdBoxVertical = 2 * verticalPlayFieldStart - lineDrawBuffer;
  TFTscreen.rect(sideBoxHorizontal, holdBoxVertical, sideBoxUnit, sideBoxUnit);
  TFTscreen.text("HOLD", sideBoxHorizontal, verticalPlayFieldStart);
  nextUpVertical = holdBoxVertical + sideBoxUnit + (2 * verticalPlayFieldStart);
  TFTscreen.text("NEXT UP", sideBoxHorizontal, holdBoxVertical + sideBoxUnit + verticalPlayFieldStart);
  TFTscreen.rect(sideBoxHorizontal, nextUpVertical, sideBoxUnit, 2 * sideBoxUnit);
}

void loop()
{

  TFTscreen.fillRect(linePosHorizontalMaxLeft, linePosVerticalMaxUp, widthPlayField, heightPlayField, Black);
  gameAlive = true;
  emptyHold = true;
  

  holdAvailable = true;
  clearGrid();

  randomSeven();
  int randomTetriminos[14];
  for (int index = 0; index < 7; index++)
  {
    randomTetriminos[index] = selectorRand[index];
  }

  randomSeven();
  for (int index = 0; index < 7; index++)
  {
    randomTetriminos[index + 7] = selectorRand[index];
  }

  while (gameAlive)
  {

    for (int index = 0; index < 7; index++)
    {
      TFTscreen.fillRect(sideBoxHorizontal+lineDrawBuffer, nextUpVertical+lineDrawBuffer, sideBoxUnit-(2* lineDrawBuffer), (2 * sideBoxUnit) - (2* lineDrawBuffer), Black);

      Tetrimino nextTet = *tetriminoArray[randomTetriminos[index+1]];
      fillInGrid(nextTet, sideBoxHorizontal + lineDrawBuffer, nextUpVertical + lineDrawBuffer, true, nextTet.colour);
           Tetrimino nextNextTet = *tetriminoArray[randomTetriminos[index+2]];
      fillInGrid(nextNextTet, sideBoxHorizontal + lineDrawBuffer, nextUpVertical +sideBoxUnit+ lineDrawBuffer, true, nextNextTet.colour);
      spawnTetrimino(*tetriminoArray[randomTetriminos[index]]);
     
      checkForClearLine();
       if (!gameAlive)
      {
        break;
      }
    }
    for (int index = 0; index < 7; index++)
    {
      randomTetriminos[index] = randomTetriminos[index + 7];
    }
    randomSeven();
    for (int index = 0; index < 7; index++)
    {
      randomTetriminos[index + 7] = selectorRand[index];
    }
  }
  TFTscreen.fillRect(sideBoxHorizontal+lineDrawBuffer, nextUpVertical+lineDrawBuffer, sideBoxUnit-(2* lineDrawBuffer), (2 * sideBoxUnit) - (2* lineDrawBuffer), Black);
    TFTscreen.fillRect(sideBoxHorizontal + lineDrawBuffer, holdBoxVertical + lineDrawBuffer, sideBoxUnit - (2* lineDrawBuffer), sideBoxUnit - (2* lineDrawBuffer), Black);
  TFTscreen.fillRect(linePosHorizontalMaxLeft, linePosVerticalMaxUp, widthPlayField, heightPlayField, Black);
  TFTscreen.stroke(White);
  TFTscreen.text("GAME OVER", (middleOfPlayField - strlen("GAME OVER")) / 2, verticalDotCentrePosition);
  delay(5000);
}

void randomSeven()
{
  int selector[7] = {0, 1, 2, 3, 4, 5, 6};
  for (int index = 0; index < 7; index++)
  {
    int randomInt = (rand() % (7 - index));
    selectorRand[index] = selector[randomInt];
    for (int subIndex = randomInt; subIndex < 6; subIndex++)
    {
      selector[subIndex] = selector[subIndex + 1];
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
          lineMultiplier++;
          needsReDoing = true;

          // Shift above lines...
          for (int u = m; u > 0; u--)
          {
            for (int r = 0; r < tetrisGridCols; r++)
            {
              playGrid[r + (tetrisGridCols * u)] = playGrid[r + (tetrisGridCols * (u - 1))];
            }
          }

          //New blank line
          for (int r = 0; r < tetrisGridCols; r++)
          {
            playGrid[r] = 0;
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
    TFTscreen.fillRect(linePosHorizontalMaxLeft, linePosVerticalMaxUp, widthPlayField, heightPlayField, Black);

    for (int u = 0; u < tetrisGridRowsIncInvis; u++)
    {
      for (int r = 0; r < tetrisGridCols; r++)
      {

        uint16_t colourOfGrid = playGrid[r + (tetrisGridCols * u)];
        if (colourOfGrid != 0)
        {
          int rationalHorizontal = ((r * multiplier) + linePosHorizontalMaxLeft + 1);

          int rationalVertical = ((u - numInvisableRows) * multiplier) + linePosVerticalMaxUp + 1;

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
  tetriminoInPlay = true;
  verticalDotPosition = verticalStartPosition;
  previousVerticalDotPosition = verticalStartPosition;
  verticalGhostPosition = verticalDotPosition;
  previousVerticalGhostPosition = verticalDotPosition;

  horizontalDotPosition = horizontalStartPosition;
  previousHorizontalDotPosition = horizontalDotPosition;

  spawnGhost(tetrimino);
  letGoOfHardDrop = false;
  previousTime = millis();
  while (tetriminoAlive && tetriminoInPlay)
  {

    moveTetrimino(tetrimino);
    timeTaken = millis() - previousTime;
    if (timeTaken >= fallSpeed)
    {
      tryToMoveDown(tetrimino);
      previousTime = millis();
    };
  }
  if (!tetriminoInPlay)
  {
    holdAvailable = false;
    // Erase the play grid tetrimino

    fillInGrid(tetrimino, horizontalDotPosition, verticalDotPosition, true, Black);
    // Erase the ghost
    ghostGrid(tetrimino, horizontalDotPosition, verticalGhostPosition, true, Black);
    // erase the hold of the old tetrimino
    TFTscreen.fillRect(sideBoxHorizontal + lineDrawBuffer, holdBoxVertical + lineDrawBuffer, sideBoxUnit - (2* lineDrawBuffer), sideBoxUnit - (2* lineDrawBuffer), Black);
    // colour in the hold with the new tetrimino
    fillInGrid(tetrimino, sideBoxHorizontal + lineDrawBuffer, holdBoxVertical + lineDrawBuffer, true, tetrimino.colour);

    if (!emptyHold)
    {
      // swap em out and spawn the next...
      Tetrimino held = holdTetrimino;
      holdTetrimino = tetrimino;
      spawnTetrimino(held);
    }
    else
    {
      holdTetrimino = tetrimino;
      emptyHold = false;
      //spawn next tetrimino
    }
  }
  else
  {
    //No longer alive... now we need to commit the block to the playing field.
    commitToPlayGrid(tetrimino);
    // reset a boolean for the hold...

    holdAvailable = true;
  }
}

void placeTetriminoInHold(Tetrimino tetrimino)
{

  // make the loop in the loop() function take on the previously allocated hold block and place the current one into hold.
  if (!emptyHold)
  {
    // Take out the hold Tetrimino and spawn it
  }
  else
  {
    // We need the next tetrimino...
  }

  // save the current in hold, make in play false.
}

void commitToPlayGrid(Tetrimino tetrimino)
{

  for (int m = 0; m < tetrimino.rows; m++)
  {

    for (int n = 0; n < tetrimino.cols; n++)
    {

      if (tetrimino.booleanOfGrid(m, n))
      {
        int rationalHorizontal = (horizontalDotPosition - linePosHorizontalMaxLeft) / multiplier;
        int rationalVertical = numInvisableRows + ((verticalDotPosition - linePosVerticalMaxUp) / multiplier);
        int col = (rationalHorizontal + m);
        int row = (rationalVertical + n);
        if (row < 2)
        {
          gameAlive = false;
        }
        playGrid[col + (tetrisGridCols * row)] = tetrimino.colour;
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
  // for (int m = 0; m < tetrisGridRowsIncInvis; m++)
  // {
  //   for (int n = 0; n < tetrisGridCols; n++)
  //   {

  //     if (playGrid[n + (tetrisGridCols * m)] != 0)
  //     {
  //       SerialUSB.print("o");
  //     }
  //     else
  //     {

  //       SerialUSB.print("-");
  //     }
  //   }
  //   SerialUSB.println();
  // }
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

  if (aButtonState == HIGH && holdAvailable)
  {
    tetriminoInPlay = false;
  }
  if (bButtonState == HIGH && letGoOfRotate)
  {

    letGoOfRotate = false;
    SerialUSB.println("ROT!");
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
  int hypotheticalVerticalDotPosition = verticalDotPosition + multiplier;

  while (!hitBottom(tetrimino, hypotheticalVerticalDotPosition) && !(overlapOfPlayGrid(tetrimino, horizontalDotPosition, hypotheticalVerticalDotPosition)))
  {
    verticalDotPosition = hypotheticalVerticalDotPosition;
    hypotheticalVerticalDotPosition = verticalDotPosition +  multiplier;
  }
  movedDown(tetrimino);
  tetriminoAlive = false;
}

void ghostPositionUpdate(Tetrimino tetrimino)
{
  verticalGhostPosition = verticalDotPosition;
  int hypotheticalVerticalGhostPosition = verticalGhostPosition + multiplier;

  while (!hitBottom(tetrimino, hypotheticalVerticalGhostPosition) && !(overlapOfPlayGrid(tetrimino, horizontalDotPosition, hypotheticalVerticalGhostPosition)))
  {
    verticalGhostPosition = hypotheticalVerticalGhostPosition;
    hypotheticalVerticalGhostPosition = verticalGhostPosition +  multiplier;
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
  int hypotheticalVerticalGhostPosition = verticalGhostPosition +  multiplier;

  while (!hitBottom(tetrimino, hypotheticalVerticalGhostPosition) && !(overlapOfPlayGrid(tetrimino, horizontalDotPosition, hypotheticalVerticalGhostPosition)))
  {
    verticalGhostPosition = hypotheticalVerticalGhostPosition;
    hypotheticalVerticalGhostPosition = verticalGhostPosition +  multiplier;
  }
  ghostColour(tetrimino);
}

void tryToMoveDown(Tetrimino tetrimino)
{
  //if the block settles above the visable line, end the game.

  int hypotheticalVerticalDotPosition = verticalDotPosition +  multiplier;
  if (hitBottom(tetrimino, hypotheticalVerticalDotPosition) || (overlapOfPlayGrid(tetrimino, horizontalDotPosition, hypotheticalVerticalDotPosition)))
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
  int hypotheticalHorizontalDotPosition = horizontalDotPosition + multiplier;

  if (hypotheticalHorizontalDotPosition + (tetrimino.blocksRight() * multiplier) > linePosHorizontalMaxRight)

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
  int hypotheticalHorizontalDotPosition = horizontalDotPosition - multiplier;

  if (hypotheticalHorizontalDotPosition + (tetrimino.blocksLeft() * multiplier) < linePosHorizontalMaxLeft)

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
        if (playGrid[(((hypotheticalHorizontalDotPosition - linePosHorizontalMaxLeft) / multiplier) + m) + (tetrisGridCols * (numInvisableRows + ((hypotheticalVerticalDotPosition - linePosVerticalMaxUp) / multiplier) + n))] != 0)
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
      if (startHorizontalDotPosition > linePosHorizontalMaxRight)
      {

        startHorizontalDotPosition = startHorizontalDotPosition + (0.5 * (4 - tetrimino.cols)*multiplier);
        if ( tetrimino.rows!=4 ){
        startVerticalDotPosition = startVerticalDotPosition + multiplier;
        }
      }

  for (int m = 0; m < tetrimino.rows; m++)
  {
    for (int n = 0; n < tetrimino.cols; n++)
    {

      boolean conditionalBoolean;
      if (startHorizontalDotPosition > linePosHorizontalMaxRight)
      {
        conditionalBoolean = tetrimino.booleanOfStartGrid(m, n);
      }
      else
      {
        conditionalBoolean = tetrimino.booleanOfGrid(m, n);
      }
      if (conditionalBoolean == trueOrFalse)
      {
        if (startVerticalDotPosition + (multiplier * n) < linePosVerticalMaxUp)
        {
        }
        else
        {
          TFTscreen.fillRect(startHorizontalDotPosition + 1 + (multiplier * m), startVerticalDotPosition + 1 + (multiplier * n), multiplier - 1, multiplier - 1, colour);
        }
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
        if (startVerticalDotPosition + (multiplier * n) < linePosVerticalMaxUp)
        {
        }
        else
        {
          TFTscreen.rect(startHorizontalDotPosition + 1 + (multiplier * m), startVerticalDotPosition + 1 + (multiplier * n), multiplier - 1, multiplier - 1);
        }
      }
    }
  }
}
