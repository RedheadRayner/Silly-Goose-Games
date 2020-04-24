
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
// #define Pink          0x780F      /* 128,   0, 128 */
#define Olive 0x7BE0     /* 128, 128,   0 */
#define LightGrey 0xC618 /* 192, 192, 192 */
#define DarkGrey 0x7BEF  /* 128, 128, 128 */
// #define Red            0x001F      /*   0,   0, 255 */
// #define Green           0x07E0      /*   0, 255,   0 */
#define Todo 0x07FF /*   0, 255, 255 */
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
int verticalDotPosition = 80;
int horizontalDotPosition = 64;

int level = 1;

// direction and speed
int speed = 1;

// variables to keep track of the point's location
int previousVerticalDotPosition = verticalDotPosition;
int previousHorizontalDotPosition = horizontalDotPosition;
int previousLevel = level;

int verticalPlayFieldStart = -1;
int horizontalPlayFieldStart = 1;
int heightPlayField = (20 * multiplier) + 2;
int widthPlayField = (10 * multiplier) + 2;

int linePosHorizontalMaxRight = horizontalPlayFieldStart + widthPlayField;
int linePosVerticalMaxUp = verticalPlayFieldStart + heightPlayField - 1;
int linePosVerticalMaxDown = verticalPlayFieldStart + 1;
int linePosHorizontalMaxLeft = horizontalPlayFieldStart;

int verticalStartPosition = linePosVerticalMaxUp + (2 * multiplier);

int horizontalStartPosition = linePosHorizontalMaxLeft + (widthPlayField / 2) - multiplier;

long timeTaken;
long previousTime;
long fallSpeed = 1000;
boolean alive;

Tetrimino *o;

// Tetrimino *l;

void setup()
{
  Serial.begin(9600);
  Serial.println("start");
  o = new Tetrimino();
  // l = new Tetrimino();

  const int oRows = 2;
  const int oCols = 2;
  boolean oArray[oRows][oCols] = {{
                                      true,
                                      true,
                                  },
                                  {
                                      true,
                                      true,
                                  }};

  boolean *oPtr[oRows];
  for (size_t i = 0; i < oRows; i++)
  {
    oPtr[i] = oArray[i];
  }
  o->initialise(oPtr, oRows, oCols, Yellow, multiplier);

  // const int lRows = 4;
  // const int lCols = 4;
  // boolean lArray[lRows][lCols] = {{false, false, false, false}, {true, true, true, true}, {false, false, false, false}, {false, false, false, false}};

  //   boolean *lPtr[lRows];
  //   for (size_t i = 0; i < lRows; i++)
  //   {
  //     lPtr[i] = lArray[i];
  //   }
  //   l->initialise(lPtr, lRows, lCols, Blue, multiplier);

  //Tetrimino line;
  //square.colour = CYAN;
  //square.grid= new boolean [4][4]  { {false,false,false,false}, {true,true,true,true}, {false,false,false,false}
  //, {false,false,false,false}};

  // initialize the pushbutton pin as an input:
  pinMode(downButtonPin, INPUT);
  pinMode(leftButtonPin, INPUT);
  pinMode(rightButtonPin, INPUT);
  pinMode(upButtonPin, INPUT);

  // Put this line at the beginning of every sketch that uses the GLCD:
  TFTscreen.begin();

  // TFTscreen.invertDisplay(0);
  // clear the screen with a black background
  TFTscreen.background(0, 0, 0);
  TFTscreen.stroke(255, 255, 255);

  //screen.rect(xStart, yStart, width, height);
  //xStart : int, the horizontal position where the line starts
  //yStart : int, the vertical position where the line starts
  //width : int, the width of the rectangle
  //height : int, the height of the rectangle
  TFTscreen.rect(verticalPlayFieldStart, horizontalPlayFieldStart, heightPlayField, widthPlayField);
}

void loop()
{
  spawnTetrimino(o);
  // spawnTetrimino(l);
}

void spawnTetrimino(Tetrimino *tetrimino)
{
  alive = true;
  // TFTscreen.stroke(255, 255, 255);
  TFTscreen.stroke(tetrimino->colour);
  int m, n, k, y;
  verticalDotPosition = verticalStartPosition;

  horizontalDotPosition = horizontalStartPosition;
  for (int m = 0; m < tetrimino->rows; m++)
  {
    for (int n = 0; n < tetrimino->cols; n++)
    {
      // There may be a way to save time here by only printing to pixels that are new...
      if (tetrimino->grid[n][m])
      {
        {
          for (y = 0; y < multiplier; y++)
          {
            for (k = 0; k < multiplier; k++)
            {
              TFTscreen.point(verticalDotPosition + (multiplier * n) + y, horizontalDotPosition + (multiplier * m) + k);
            }
          }
        }
      }
    }
  }

  // some coloured pixels are being printed off screen and due to these being new, they are not coloured again...
  Serial.println(horizontalDotPosition + speed + (tetrimino->blocksRight() * multiplier));
  previousTime = millis();
  while (alive)
  {
    moveTetrimino(tetrimino);
    timeTaken = millis() - previousTime;
    if (timeTaken >= fallSpeed)
    {
      forceDownTetrimino(tetrimino);
      previousTime = millis();
    };
  }
}

void forceDownTetrimino(Tetrimino *tetrimino)
{
  if (verticalDotPosition - ((speed+tetrimino->blocksDown()) * multiplier) < linePosVerticalMaxDown)
  {

    alive = false;
  }
  else
  {
    long timeBefore = millis();
    int multi, k;
    for (multi = 0; multi < multiplier; multi++)
    {
      verticalDotPosition = verticalDotPosition - speed;
      for (int n = 0; n < tetrimino->cols; n++)
      {

        for (k = 0; k < multiplier; k++)
        {
          // Serial.println("colour in");
          TFTscreen.stroke(0, 0, 0);
          TFTscreen.point(previousVerticalDotPosition, horizontalDotPosition + (multiplier * n) + k); // color in the old point
          if (tetrimino->grid[tetrimino->rows - 1][n])
          {
            TFTscreen.stroke(tetrimino->colour);
            TFTscreen.point(verticalDotPosition - (multiplier * tetrimino->rows), horizontalDotPosition + (multiplier * n) + k); // color in the new point
          }
        }
      }

      previousVerticalDotPosition = verticalDotPosition;
    }
    long timeTook = millis() - timeBefore;
    // Serial.println(timeTook);
  }

  delay(5);
}

void moveTetrimino(Tetrimino *tetrimino)
{
  int m, n;
  downButtonState = digitalRead(downButtonPin);
  upButtonState = digitalRead(upButtonPin);
  leftButtonState = digitalRead(leftButtonPin);
  rightButtonState = digitalRead(rightButtonPin);

  if (downButtonState == HIGH)
  {
    //TODO - implement faster fall

    if (verticalDotPosition - speed - (tetrimino->blocksDown()*multiplier) <= linePosVerticalMaxDown)
    {

      Serial.println("MAX_DOWN");
    }
    else
    {
      int multi, k;
      for (multi = 0; multi < multiplier; multi++)
      {
        verticalDotPosition = verticalDotPosition - speed;
        for (int n = 0; n < tetrimino->cols; n++)
        {

          for (k = 0; k < multiplier; k++)
          {
            TFTscreen.stroke(0, 0, 0);
            TFTscreen.point(previousVerticalDotPosition, horizontalDotPosition + (multiplier * n) + k); // color in the old point
            if (tetrimino->grid[tetrimino->rows - 1][n])
            {
              TFTscreen.stroke(tetrimino->colour);
              TFTscreen.point(verticalDotPosition - (multiplier * tetrimino->rows), horizontalDotPosition + (multiplier * n) + k); // color in the new point
            }
          }
        }
        previousVerticalDotPosition = verticalDotPosition;
      }
    }
  }
  if (upButtonState == HIGH)
  {
    //Cannot go up- implement hard drop...

    // if (verticalDotPosition + speed == linePosVerticalMaxUp)
    // {
    //   Serial.println("MAX_UP");
    // }
    // else
    // {
    //   verticalDotPosition = verticalDotPosition + speed;
    //   for (int n = 0; n < tetrimino->cols; n++)
    //   {
    //     TFTscreen.stroke(0, 0, 0);
    //     TFTscreen.point(previousVerticalDotPosition - tetrimino->rows, previousHorizontalDotPosition + n); // color in the old point
    //     TFTscreen.stroke(tetrimino->colour);
    //     TFTscreen.point(verticalDotPosition, previousHorizontalDotPosition + n); // color in the new point
    //   }
    // }
  }
  if (leftButtonState == HIGH)
  {

    if (horizontalDotPosition - speed <= linePosHorizontalMaxLeft)
    {
      Serial.println("MAX_LEFT");
    }
    else
    {

      int multi, k;
      for (multi = 0; multi < multiplier; multi++)
      {
        horizontalDotPosition = horizontalDotPosition - speed;

        for (int m = 0; m < tetrimino->rows; m++)
        {

          for (k = 0; k < multiplier; k++)
          {
            TFTscreen.stroke(0, 0, 0);
            TFTscreen.point(verticalDotPosition - (multiplier * m) - k, previousHorizontalDotPosition + (multiplier * tetrimino->cols)); // color in the old point
            if (tetrimino->grid[m][0])
            {
              TFTscreen.stroke(tetrimino->colour);
              TFTscreen.point(verticalDotPosition - (multiplier * m) - k, horizontalDotPosition); // color in the new point
            }
          }
        }

        previousHorizontalDotPosition = horizontalDotPosition;
      }
    }
  }
  if (rightButtonState == HIGH)
  {
    Serial.println(horizontalDotPosition);

    if (horizontalDotPosition + speed + (tetrimino->blocksRight() * multiplier) >= linePosHorizontalMaxRight)

    {
      Serial.println("MAX_RIGHT");
    }
    else
    {

      int multi, k;
      for (multi = 0; multi < multiplier; multi++)
      {
        horizontalDotPosition = horizontalDotPosition + speed;
        for (int m = 0; m < tetrimino->rows; m++)
        {

          for (k = 0; k < multiplier; k++)
          {

            TFTscreen.stroke(0, 0, 0);
            TFTscreen.point(verticalDotPosition - (multiplier * m) - k, previousHorizontalDotPosition); // color in the old point

            if (tetrimino->grid[m][tetrimino->cols - 1])
            {
              TFTscreen.stroke(tetrimino->colour);
              TFTscreen.point(verticalDotPosition - (multiplier * m) - k, horizontalDotPosition + (multiplier * tetrimino->cols)); // color in the new point
            }
          }
        }
        previousHorizontalDotPosition = horizontalDotPosition;
      }
    }
  }
  previousLevel = level;
  delay(50);
}

double calculateFallSpeed(int level)
{
  return pow(0.8 - ((level - 1) * 0.007), (level - 1));
}