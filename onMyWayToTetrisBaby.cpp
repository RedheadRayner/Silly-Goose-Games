
#include <SD.h>
#include <TFT.h> // Arduino LCD library
#include <SPI.h>
#include <Arduino.h>
// pin definition for the Uno
// #define cs 10
// #define dc 9
// #define rst 8
#include "tetrimino.h"
#include <EEPROM.h>
// #include <SdFat.h>
// #include <Arduino_ST7735_STM.h>

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
#define Blue 0xFFE0
#define White 0xFFFF
#define Blue 0xFD20
#define GreenYellow 0xAFE5

int multiplier;

// create an instance of the library
TFT  TFTscreen = TFT (TFT_CS, TFT_DC, TFT_RST);

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

int level;

// direction and speed
int speed;

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
int clearedLines;
long timeTaken;
long previousTime;
long fallSpeed;
boolean tetriminoAlive;
boolean tetriminoInPlay;
boolean gameAlive;
boolean letGoOfHardDrop;
boolean letGoOfRotate;
int *currentTetriminoGrid;

Tetrimino o_tetrimino;
Tetrimino l_tetrimino;
Tetrimino t_tetrimino;
Tetrimino i_tetrimino;
Tetrimino j_tetrimino;
Tetrimino s_tetrimino;
Tetrimino z_tetrimino;

Tetrimino holdTetrimino;

Tetrimino *tetriminoArray[7] = {&o_tetrimino, &l_tetrimino, &t_tetrimino, &i_tetrimino, &j_tetrimino, &s_tetrimino, &z_tetrimino};

uint16_t playGrid[tetrisGridCols * tetrisGridRowsIncInvis];

int sideBoxUnitHorizontal;
int sideBoxUnitVertical;
int holdBoxVertical;
int scoreTextVertical;

int levelTextVertical;

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
void tryToMoveDown(Tetrimino tetrimino, boolean incrementScore);
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
void checkForLevelUp();

void updateScore();
void updateLevel();
boolean emptyHold;
boolean holdAvailable;
int selectorRand[7];
int score;

 #define SD_MOSI PB15
 #define SD_MISO PB14
 #define SD_SCK  PB13
 #define SD_CS   PB12

//    SPIClass SPI_2(SD_MOSI,SD_MISO, SD_SCK,SD_CS);
// // SdSpiCard card();
// SdFat sd(&SPI_2);

// SdFile file;
#define NLINES 32
#define BUF_WD 160
uint16_t buf[BUF_WD*NLINES] __attribute__((aligned(4)));

// int showBMP(char *filename)
// {
//   int bmpWd, bmpHt, bmpBits, bmpNumCols, y=0;
//   uint16_t pal[256];
  
//   if(!file.open(filename, O_CREAT | O_RDONLY)) {
//     TFTscreen.fillScreen(Yellow);
//     Serial.print(F("Cannot open "));
//     Serial.println(filename);
//     delay(1000);
//     //return -1;
//   }
//   file.seekSet(0);
//   file.read(buf,54);
//   uint8_t *buf8 = (uint8_t *)buf;
//   bmpWd = buf8[18]+buf8[19]*256;
//   bmpHt = buf8[22]+buf8[23]*256;
//   bmpBits = buf8[28];
//   bmpNumCols = buf8[46]+buf8[47]*256;
//   if(bmpBits<=8) {
//     file.read(buf,bmpNumCols*4);
//     for(int i=0;i<bmpNumCols;i++) pal[i]=RGBto565(buf8[2+i*4],buf8[1+i*4],buf8[i*4]);
//   }
//   while(file.available() && y<bmpHt) {
//     buf8 = (uint8_t *)buf+BUF_WD*2;
//     if(bmpBits==4) {
//       file.read(buf8,bmpWd/2);
//       for(int i=0;i<bmpWd/2;i++) {
//         buf[i*2+0] = pal[buf8[i]>>4];
//         buf[i*2+1] = pal[buf8[i]&0xf];
//       }
//     } else
//     if(bmpBits==8) {
//       file.read(buf8,bmpWd);
//       for(int i=0;i<bmpWd;i++) buf[i] = pal[buf8[i]];
//     } else {
//       file.read(buf8,bmpWd*3);
//       for(int i=0;i<bmpWd;i++) buf[i] = RGBto565(buf8[i*3+2],buf8[i*3+1],buf8[i*3+0]);
//     }
//     TFTscreen.drawBitmap()
//      TFTscreen.drawImage((TFTscreen.width()-bmpWd)/2,TFTscreen.height()-1-y,bmpWd,1,buf);
//     y++;
//   }
//   file.close();
//   return 1;
// }
void setup()
{
  
    SerialUSB.begin();
  delay(2000);
  // Put this line at the beginning of every sketch that uses the GLCD:
  TFTscreen.begin();
  TFTscreen.setRotation(0);
  TFTscreen.background(Red);


  delay(2000);
SerialUSB.println("Beginning card...");
// if ( !sd.cardBegin(SD_CS, SD_SCK_MHZ(18))){
//   SerialUSB.println("Card has not begun!");
//     while(1);
// }
//   if(!sd.fsBegin()) {
//   SerialUSB.println("File system has not begun!");
//     while(1);
// }
// if(!sd.ls()){
//   SerialUSB.println("HAHAHAHA");
// }


  SD.begin(PB12, PB15, PB14, PB13);
 
  


PImage parrotImage = TFTscreen.loadImage("PARROT.BMP");
TFTscreen.image(parrotImage,0,0);


 File parrot = SD.open("PARROT.BMP", O_READ); 

    SerialUSB.println("show parrot...");

if (!parrot.available()){
  SerialUSB.println("Parrot is off limits!");
    while(1);
}
  
  // // SerialUSB.write(parrot.read());
  // SerialUSB.println(parrot.available());

//   int16_t byte, x = 0, y = 0;
//   while ((byte = parrot.read()) != -1)
//   {
//     int16_t byte2 = parrot.read();
//     uint16_t color = (byte2 << 8) | (byte & 0xff);
//     TFTscreen.drawPixel(x, y, color);
//     x++;
//     SerialUSB.printf("%x\r\n", color);
//     if (x > TFT_MAX_WIDTH)
//     {
//       x = 0;
//       y++;
//       // while (1)
//       //   ;
//     }
//   }
// parrot.close();
  delay(5000);

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

  o_tetrimino.initialise(oArray, oRows, oCols, Yellow, multiplier);

  const int iRows = 4;
  const int iCols = 4;
  int iArray[iRows * iCols] = {0, 0, 0, 0,
                               1, 1, 1, 1,
                               0, 0, 0, 0,
                               0, 0, 0, 0};

  i_tetrimino.initialise(iArray, iRows, iCols, Blue, multiplier);

  const int tRows = 3;
  const int tCols = 3;
  int tArray[tRows * tCols] = {0, 1, 0,
                               1, 1, 1,
                               0, 0, 0};

  t_tetrimino.initialise(tArray, tRows, tCols, Pink, multiplier);

  const int lRows = 3;
  const int lCols = 3;
  int lArray[lRows * lCols] = {0, 0, 1,
                               1, 1, 1,
                               0, 0, 0};

  l_tetrimino.initialise(lArray, lRows, lCols, Orange, multiplier);

  const int jRows = 3;
  const int jCols = 3;
  int jArray[jRows * jCols] = {1, 0, 0,
                               1, 1, 1,
                               0, 0, 0};

  j_tetrimino.initialise(jArray, jRows, jCols, DarkBlue, multiplier);

  const int sRows = 3;
  const int sCols = 3;
  int sArray[sRows * sCols] = {0, 1, 1,
                               1, 1, 0,
                               0, 0, 0};

  s_tetrimino.initialise(sArray, sRows, sCols, Green, multiplier);

  const int zRows = 3;
  const int zCols = 3;
  int zArray[zRows * zCols] = {1, 1, 0,
                               0, 1, 1,
                               0, 0, 0};

  z_tetrimino.initialise(zArray, zRows, zCols, Red, multiplier);

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
  TFTscreen.setTextSize(1);
  sideBoxUnitHorizontal = (4 * multiplier) + (2 * lineDrawBuffer);
  sideBoxUnitVertical = (3 * multiplier) + (2 * lineDrawBuffer);
  sideBoxHorizontal = ((screenShort + (widthPlayFieldContainer + horizontalPlayFieldStart) - sideBoxUnitHorizontal) / 2) - lineDrawBuffer;
  holdBoxVertical = verticalPlayFieldStart + multiplier;
  TFTscreen.rect(sideBoxHorizontal, holdBoxVertical, sideBoxUnitHorizontal, sideBoxUnitVertical);
  TFTscreen.text("HOLD", sideBoxHorizontal, verticalPlayFieldStart);

  nextUpVertical = holdBoxVertical + sideBoxUnitVertical + (2 * multiplier);
  TFTscreen.text("NEXT", sideBoxHorizontal, holdBoxVertical + sideBoxUnitVertical + multiplier);
  TFTscreen.rect(sideBoxHorizontal, nextUpVertical, sideBoxUnitHorizontal, 2 * sideBoxUnitVertical);

  scoreTextVertical = nextUpVertical + (2 * sideBoxUnitVertical) + (2.5 * multiplier);

  TFTscreen.text("SCORE", sideBoxHorizontal, nextUpVertical + (2 * sideBoxUnitVertical) + multiplier);
  TFTscreen.text("0", sideBoxHorizontal, scoreTextVertical);

  levelTextVertical = nextUpVertical + (2 * sideBoxUnitVertical) + (6 * multiplier);

  TFTscreen.text("LEVEL", sideBoxHorizontal, nextUpVertical + (2 * sideBoxUnitVertical) + (4.5 * multiplier));
  TFTscreen.text("1", sideBoxHorizontal, levelTextVertical);
}

void loop()
{

  TFTscreen.fillRect(linePosHorizontalMaxLeft, linePosVerticalMaxUp, widthPlayField, heightPlayField, Black);
  gameAlive = true;
  emptyHold = true;
  clearedLines = 0;
  level = 1;
  speed = 1;
  fallSpeed = 1000;
  score = 0;
  updateScore();
  updateLevel();

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
      TFTscreen.fillRect(sideBoxHorizontal + lineDrawBuffer, nextUpVertical + lineDrawBuffer, sideBoxUnitHorizontal - (2 * lineDrawBuffer), (2 * sideBoxUnitVertical) - (2 * lineDrawBuffer), Black);

      Tetrimino nextTet = *tetriminoArray[randomTetriminos[index + 1]];
      fillInGrid(nextTet, sideBoxHorizontal + lineDrawBuffer, nextUpVertical + lineDrawBuffer, true, nextTet.colour);
      Tetrimino nextNextTet = *tetriminoArray[randomTetriminos[index + 2]];
      fillInGrid(nextNextTet, sideBoxHorizontal + lineDrawBuffer, nextUpVertical + sideBoxUnitVertical + lineDrawBuffer, true, nextNextTet.colour);
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
  TFTscreen.fillRect(sideBoxHorizontal + lineDrawBuffer, nextUpVertical + lineDrawBuffer, sideBoxUnitHorizontal - (2 * lineDrawBuffer), (2 * sideBoxUnitVertical) - (2 * lineDrawBuffer), Black);
  TFTscreen.fillRect(sideBoxHorizontal + lineDrawBuffer, holdBoxVertical + lineDrawBuffer, sideBoxUnitHorizontal - (2 * lineDrawBuffer), sideBoxUnitVertical - (2 * lineDrawBuffer), Black);
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

void checkForLevelUp()
{
  if (clearedLines >= level * 5)
  {
    clearedLines = clearedLines - (level * 5);
    //level up!
    level++;
    fallSpeed = calculateFallSpeed(level);
    updateLevel();
  }
}

void updateScore()
{
  TFTscreen.fillRect(sideBoxHorizontal, scoreTextVertical, sideBoxUnitHorizontal, multiplier, Black);
  TFTscreen.stroke(White);
  std::string str = std::to_string(score);
  const char *cstr2 = str.c_str();
  TFTscreen.text(cstr2, sideBoxHorizontal, scoreTextVertical);
}

void updateLevel()
{
  TFTscreen.fillRect(sideBoxHorizontal, levelTextVertical, sideBoxUnitHorizontal, multiplier, Black);
  TFTscreen.stroke(White);
  std::string str = std::to_string(level);
  const char *cstr2 = str.c_str();
  TFTscreen.text(cstr2, sideBoxHorizontal, levelTextVertical);
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
    int lineScoreMutliplier = 1 + (2 * (lineMultiplier - 1));
    clearedLines = clearedLines + lineScoreMutliplier;
    score = score + (100 * lineScoreMutliplier * level);
    updateScore();
    checkForLevelUp();
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
      tryToMoveDown(tetrimino, false);
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
    TFTscreen.fillRect(sideBoxHorizontal + lineDrawBuffer, holdBoxVertical + lineDrawBuffer, sideBoxUnitHorizontal - (2 * lineDrawBuffer), sideBoxUnitVertical - (2 * lineDrawBuffer), Black);
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
    tryToMoveDown(tetrimino, true);
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
  int counter = 0;
  while (!hitBottom(tetrimino, hypotheticalVerticalDotPosition) && !(overlapOfPlayGrid(tetrimino, horizontalDotPosition, hypotheticalVerticalDotPosition)))
  {
    verticalDotPosition = hypotheticalVerticalDotPosition;
    hypotheticalVerticalDotPosition = verticalDotPosition + multiplier;
    counter++;
  }
  movedDown(tetrimino);
  tetriminoAlive = false;
  score = score + (2 * counter);
  updateScore();
}

void ghostPositionUpdate(Tetrimino tetrimino)
{
  verticalGhostPosition = verticalDotPosition;
  int hypotheticalVerticalGhostPosition = verticalGhostPosition + multiplier;

  while (!hitBottom(tetrimino, hypotheticalVerticalGhostPosition) && !(overlapOfPlayGrid(tetrimino, horizontalDotPosition, hypotheticalVerticalGhostPosition)))
  {
    verticalGhostPosition = hypotheticalVerticalGhostPosition;
    hypotheticalVerticalGhostPosition = verticalGhostPosition + multiplier;
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
  int hypotheticalVerticalGhostPosition = verticalGhostPosition + multiplier;

  while (!hitBottom(tetrimino, hypotheticalVerticalGhostPosition) && !(overlapOfPlayGrid(tetrimino, horizontalDotPosition, hypotheticalVerticalGhostPosition)))
  {
    verticalGhostPosition = hypotheticalVerticalGhostPosition;
    hypotheticalVerticalGhostPosition = verticalGhostPosition + multiplier;
  }
  ghostColour(tetrimino);
}

void tryToMoveDown(Tetrimino tetrimino, boolean incrementScore)
{
  //if the block settles above the visable line, end the game.

  int hypotheticalVerticalDotPosition = verticalDotPosition + multiplier;
  if (hitBottom(tetrimino, hypotheticalVerticalDotPosition) || (overlapOfPlayGrid(tetrimino, horizontalDotPosition, hypotheticalVerticalDotPosition)))
  {
    tetriminoAlive = false;
  }
  else
  {
    verticalDotPosition = hypotheticalVerticalDotPosition;
    movedDown(tetrimino);
    if (incrementScore)
    {
      score++;
      updateScore();
    }
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
  return 1000 * pow(0.8 - ((level - 1) * 0.007), (level - 1));
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

    startHorizontalDotPosition = startHorizontalDotPosition + (0.5 * (4 - tetrimino.cols) * multiplier);
    if (tetrimino.rows != 4)
    {
      startVerticalDotPosition = startVerticalDotPosition + multiplier;
      startVerticalDotPosition = startVerticalDotPosition - (0.5 * multiplier);
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
