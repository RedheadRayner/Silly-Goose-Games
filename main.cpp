
#include <SD.h>
#include <TFT.h> // Arduino LCD library
#include <SPI.h>
#include "main.h"
#include <Arduino.h>
#include "tetrimino.h"

/* TFT screen pin definitions */
// TFT screen CS pin definition
#define TFT_CS PB10

// TFT screen DC pin definition
#define TFT_DC PB0

// TFT screen RESET pin definition
#define TFT_RST PB1

/* SD card pin definitions */
// SD MOSI pin definition
#define SD_MOSI PB15

// SD MISO pin definition
#define SD_MISO PB14

// SD SCK pin definition
#define SD_SCK PB13

// SD CS pin definition
#define SD_CS PB12


/* push button pin definitions */
// down button pin definition
#define DOWN_BUTTON PA2
// up button pin definition
#define UP_BUTTON PA4

// right button pin definition
#define RIGHT_BUTTON PA6

// left button pin definition
#define LEFT_BUTTON PA3

// b button pin definition
#define B_BUTTON PA1

// a button pin definition
#define A_BUTTON PA0

// Color definitions
/* some RGB color definitions                                                 */
#define Black 0x0000
#define Green 0x03E0
#define Yellow 0x03EF
#define DarkBlue 0x7800
#define Pink 0x780F
#define LightGrey 0xC618
#define DarkGrey 0x7BEF
#define Red 0x001F
#define Orange 0x02EF
#define Green 0x07E0
#define White 0xFFFF
#define Blue 0xFD20
#define GreenYellow 0xAFE5

// the multiplier to scale the gameplay depending on the screen resolution
int multiplier;

// the scale for the text depending on the screen resolution
int textMultiplier;

// create an instance of the library for the TFT screen
TFT TFTscreen = TFT(TFT_CS, TFT_DC, TFT_RST);

// variable for reading the down button status
int downButtonState = 0;  

// variable for reading the left button status
int leftButtonState = 0;  

 // variable for reading the right button status
int rightButtonState = 0;

// variable for reading the up button status
int upButtonState = 0;    

// variable for reading the b button status
int bButtonState = 0;   

// variable for reading the a button status
int aButtonState = 0;

// the height of the screen (the shortest dimension of the screen)
int16_t screenShort = TFTscreen.height();

// the width of the screen (the longest dimension of the screen)
int16_t screenLong = TFTscreen.width();

// y position of the point in the middle of the screen
int verticalDotCentrePosition;

// x position of the point in the middle of the screen
int horizontalDotCentrePosition;

// the level of the player (to determine fall speed)
int level;

// direction and speed
int speed;

// pixel height of text on the screen at text size 1
int textHeight;

// pixel width of text on the screen at text size 1
int textWidth;

// a buffer for when drawing boxes
const int lineDrawBuffer = 1;

// the number of invisable lines at the top of the playgrid
const int numInvisableRows = 2;

// the number of columns in the playgrid
const int tetrisGridCols = 10;

// the number of rows in the playgrid
const int tetrisGridRows = 20;

// the number of rows in the playgrid when including the invisible rows
const int tetrisGridRowsIncInvis = tetrisGridRows + numInvisableRows;

// the y position of the start (top) playfield
int verticalPlayFieldStart;

// the x position of the start (lefthand side) playfield
int horizontalPlayFieldStart;

// height of the playgrid container in pixels
int heightPlayFieldContainer;

// width of the playgrid container in pixels
int widthPlayFieldContainer;

// height of the playgrid in pixels
int heightPlayField;

// width of the playgrid in pixels
int widthPlayField;

// the x position of the rightmost point in the playgrid
int linePosHorizontalMaxRight;

// the y position of the upmost point in the playgrid
int linePosVerticalMaxUp;

// the y position of the bottommost point in the playgrid
int linePosVerticalMaxDown;

// the x position of the leftmost point in the playgrid
int linePosHorizontalMaxLeft;

// the y position of the start point in the playgrid
int verticalStartPosition;

// the x position of the middle point in the playgrid
int middleOfPlayField;

// the x position of the start point in the playgrid
int horizontalStartPosition;

// variables to keep track of the point's location
// the current vertical position of the tetrimino grid
int verticalDotPosition;

// the current horizontal position of the tetrimino grid
int horizontalDotPosition;

// the previous vertical position of the tetrimino grid
int previousVerticalDotPosition;

// the previous horizontal position of the tetrimino grid
int previousHorizontalDotPosition;

// the current vertical position of the ghost tetrimino grid
int verticalGhostPosition;

// the previous vertical position of the ghost tetrimino grid
int previousVerticalGhostPosition;

// the number of cleared lines for checking for level up
int clearedLines;

// the time at which a tetrimino makes a one block fall
long fallSpeed;

// whether the tetrimino is still alive to manipulate
bool tetriminoAlive;

// whether the tetrimino is still in play (else, in the hold)
bool tetriminoInPlay;

// whether the game is still alive to play (else, game over)
bool gameAlive;

// whether the hard drop button has been released
bool letGoOfHardDrop;

// whether the rotate button has been released
bool letGoOfRotate;

// the current tetrimino grid in play (to check the validity of tetrimino movement)
int *currentTetriminoGrid;

/* the tetriminos */
// the o tetrimino
Tetrimino o_tetrimino;

// the l tetrimino
Tetrimino l_tetrimino;

// the t tetrimino
Tetrimino t_tetrimino;

// the i tetrimino
Tetrimino i_tetrimino;

// the j tetrimino
Tetrimino j_tetrimino;

// the s tetrimino
Tetrimino s_tetrimino;

// the z tetrimino
Tetrimino z_tetrimino;

// the tetrimino in the hold cell
Tetrimino holdTetrimino;

// the 7 tetriminos in an array to be chosen from
Tetrimino *tetriminoArray[7] = {&o_tetrimino, &l_tetrimino, &t_tetrimino, &i_tetrimino, &j_tetrimino, &s_tetrimino, &z_tetrimino};

// the play grid (and the colour of the blocks)
uint16_t playGrid[tetrisGridCols * tetrisGridRowsIncInvis];

// the hold and next up box horizontal size
int sideBoxUnitHorizontal;

// the next up box vertical size
int sideBoxUnitVertical;

// the hold box vertical size
int holdBoxVertical;

// the vertical position of the score text
int scoreTextVertical;

// the vertical position of the level text
int levelTextVertical;

// the vertical position of the next up text
int nextUpVertical;

// the horizontal position of the side boxes
int sideBoxHorizontal;

// whether the hold is empty or contains a tetrimino
bool emptyHold;

// whether the current tetrimino is allowed into the hold (if it just came out of the hold, it cannot immediately be placed back in the hold)
bool holdAvailable;

// the randomiser for the selection of tetrimino
int selectorRand[7];

// the current score
int score;



File scoreFile;

void setup()
{
 
  SerialUSB.begin();
  TFTscreen.begin();
  TFTscreen.setRotation(0);

  SD.begin(SD_CS, SD_MOSI, SD_MISO, SD_SCK);


  multiplier = 1;
  while (((float)screenLong / ((float)(tetrisGridRows * (multiplier + 1)))) > 1.0)
  {
    multiplier++;
  }

  textMultiplier = multiplier / 7;

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

  int oArray[4] = {
      1,
      1,
      1,
      1,
  };

  o_tetrimino.initialise(oArray, 2, Yellow, multiplier);

  int iArray[16] = {0, 0, 0, 0,
                    1, 1, 1, 1,
                    0, 0, 0, 0,
                    0, 0, 0, 0};

  i_tetrimino.initialise(iArray, 4, Blue, multiplier);

  int tArray[9] = {0, 1, 0,
                   1, 1, 1,
                   0, 0, 0};

  t_tetrimino.initialise(tArray, 3, Pink, multiplier);

  int lArray[9] = {0, 0, 1,
                   1, 1, 1,
                   0, 0, 0};

  l_tetrimino.initialise(lArray, 3, Orange, multiplier);

  int jArray[9] = {1, 0, 0,
                   1, 1, 1,
                   0, 0, 0};

  j_tetrimino.initialise(jArray, 3, DarkBlue, multiplier);

  int sArray[9] = {0, 1, 1,
                   1, 1, 0,
                   0, 0, 0};

  s_tetrimino.initialise(sArray, 3, Green, multiplier);

  int zArray[9] = {1, 1, 0,
                   0, 1, 1,
                   0, 0, 0};

  z_tetrimino.initialise(zArray, 3, Red, multiplier);
  pinMode(PB11, OUTPUT);
  analogWrite(PB11, 100);


  pinMode(DOWN_BUTTON, INPUT);
  pinMode(UP_BUTTON, INPUT);
  pinMode(LEFT_BUTTON, INPUT);
  pinMode(RIGHT_BUTTON, INPUT);
  pinMode(B_BUTTON, INPUT);
  pinMode(A_BUTTON, INPUT);

  textHeight = textMultiplier * 8;
  textWidth = textMultiplier * 6;

  sideBoxUnitHorizontal = (4 * multiplier) + (2 * lineDrawBuffer);
  sideBoxUnitVertical = (3 * multiplier) + (2 * lineDrawBuffer);
  sideBoxHorizontal = ((screenShort + (widthPlayFieldContainer + horizontalPlayFieldStart) - sideBoxUnitHorizontal) / 2) - lineDrawBuffer;
  holdBoxVertical = verticalPlayFieldStart + multiplier;

  nextUpVertical = holdBoxVertical + sideBoxUnitVertical + (2 * multiplier);
  scoreTextVertical = nextUpVertical + (2 * sideBoxUnitVertical) + (2.5 * multiplier);
  levelTextVertical = nextUpVertical + (2 * sideBoxUnitVertical) + (6 * multiplier);

  TFTscreen.setTextSize(textMultiplier);
  scoreFile = SD.open("scores.txt", FILE_WRITE);
  scoreFile.close();
  if (!SD.exists("scores.txt"))
  {
    errorScreen("SCORES FILE BROKEN");
  }
}

void loop()
{
  enterMenu();
}

void readButtonStates()
{
  downButtonState = digitalRead(DOWN_BUTTON);
  upButtonState = digitalRead(UP_BUTTON);
  leftButtonState = digitalRead(LEFT_BUTTON);
  rightButtonState = digitalRead(RIGHT_BUTTON);
  bButtonState = digitalRead(B_BUTTON);
  aButtonState = digitalRead(A_BUTTON);
}

// Main menu selector
void enterMenu()
{
  int selected = menuSetup();
  switch (selected)
  {
  case 0:
    enterGame();
    break;
  case 1:
    enterScores();
    break;
  case 2:
    enterSettings();
    break;
  default:
    errorScreen("INVALID INT");
  }
}

// Begins the tetris gameplay
void enterGame()
{
  gameScreenSetup();

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
  gameOver();
  delay(1000);
  enterGameEndedPhase();
}

// Allows the player to create a name to save to the scoreboard.
char *newPlayer()
{
    TFTscreen.background(Black);
  structuredSelectableText("ENTER", verticalDotCentrePosition - (7 * textHeight), false, true, false);
  structuredSelectableText("YOUR", verticalDotCentrePosition - (5 * textHeight), false, true, false);
  structuredSelectableText("NAME", verticalDotCentrePosition - (3 * textHeight), false, true, false);
  char *characters[27] = {" ", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"};
  int letters[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  int charactSelec = 0;
  makeSelectionScoreArrows();
  bool letGoOfA = true;
  bool letGoOfB = true;

  textNameUnderscore(charactSelec, Green);

  while (true)
  {
    readButtonStates();

    if (downButtonState == HIGH)
    {
      if (letters[charactSelec] == 26)
      {
        letters[charactSelec] = 0;
      }
      else
      {
        int newInt = letters[charactSelec] + 1;
        letters[charactSelec] = newInt;
      }

      changeSelectionScoreName(charactSelec, characters[letters[charactSelec]], Green);
      changeSelectionScoreArrows(charactSelec, false, Green);
    }
    if (upButtonState == HIGH)
    {
      if (letters[charactSelec] == 0)
      {
        letters[charactSelec] = 26;
      }
      else
      {
        int newInt = letters[charactSelec] - 1;
        letters[charactSelec] = newInt;
      }

      changeSelectionScoreName(charactSelec, characters[letters[charactSelec]], Green);
      changeSelectionScoreArrows(charactSelec, true, Green);
    }
    if (upButtonState == LOW)
    {
      changeSelectionScoreArrows(charactSelec, true, White);
    }
    if (downButtonState == HIGH)
    {
      changeSelectionScoreArrows(charactSelec, false, White);
    }

    if (aButtonState == HIGH && letGoOfA)
    {
      letGoOfA = false;
      changeSelectionScoreName(charactSelec, characters[letters[charactSelec]], White);
      textNameUnderscore(charactSelec, Black);
      if (charactSelec == 7)
      {
        break;
      }
      else
      {
        charactSelec++;
        textNameUnderscore(charactSelec, Green);
      }
    }

    if (bButtonState == HIGH && letGoOfB)
    {
      letGoOfB = false;

      if (charactSelec != 0)
      {
        textNameUnderscore(charactSelec, Black);
        changeSelectionScoreName(charactSelec, characters[letters[charactSelec]], White);
        charactSelec--;
        textNameUnderscore(charactSelec, Green);
      }
    }
    if (aButtonState == LOW)
    {
      letGoOfA = true;
    }
    if (bButtonState == LOW)
    {
      letGoOfB = true;
    }

    delay(200);
  }

  char *name;
  name = (char *)malloc(9);
  for (int i = 0; i < 8; i++)
  {
    name[i] = *characters[letters[i]];
  }
  name[8] = '\0';

  return name;
}

// Displays the player ranking 
void enterGameEndedPhase()
{

  TFTscreen.background(Black);
  char *name = newPlayer();
  TFTscreen.background(Black);
  structuredSelectableText("PLEASE", verticalDotCentrePosition - (2 * textHeight), false, true, false);
  structuredSelectableText("WAIT", verticalDotCentrePosition, false, true, false);

  int scoreLinesCounting = 0;
  String restOfFile = "";
  int place = 1;
  scoreFile = SD.open("scores.txt");

  while (scoreFile.available() && place < 1001)
  {
    String string = scoreFile.readStringUntil(',');
    int scoreLineLength = string.length();
    char *nameInBoard;
    nameInBoard = (char *)malloc(9);
    for (int i = 0; i < 8; i++)
    {
      nameInBoard[i] = string[i];
    }
    nameInBoard[8] = '\0';

    int actualIntScore = 0;

    for (int i = 0; i < scoreLineLength - 8; i++)
    {
      int tenthPow = 1;
      for (int j = 1; j < (scoreLineLength - 8 - i); j++)
      {
        tenthPow = tenthPow * 10;
      }

      actualIntScore = actualIntScore + (tenthPow * (string[i + 8] - '0'));
    }

    if (score <= actualIntScore)
    {
      scoreLinesCounting =
          scoreLinesCounting + scoreLineLength + 1;
      place++;
    }
    else
    {
      restOfFile = string + ',' + scoreFile.readString();

      break;
    }

    free(nameInBoard);
  }

  scoreFile.close();
  scoreFile = SD.open("scores.txt", FILE_WRITE);
  scoreFile.seek(scoreLinesCounting);
  scoreFile.print(name);
  scoreFile.print(score);
  scoreFile.print(",");
  scoreFile.print(restOfFile);
  scoreFile.close();
  TFTscreen.background(Black);

  structuredSelectableText("YOU ARE", verticalDotCentrePosition - (3 * textHeight), false, true, false);

  char *positionText = position(place);
  structuredSelectableText(positionText, verticalDotCentrePosition, false, true, false);
 free(positionText);
  free(name);

  structuredSelectableText("PRESS B", verticalDotCentrePosition + (6 * textHeight), false, false, false);
  structuredSelectableText("FOR SCORES", verticalDotCentrePosition + (7 * textHeight), false, false, false);
  waitForB();
  scoreBoard(true);
}

char* position(int place){
char* suffix;
  int lastDigit = place % 10;
 if(lastDigit==1){
   suffix = "st";
 }else if( lastDigit ==2){
suffix = "nd";
 }else if (lastDigit == 3){
suffix = "rd";
 }else {
suffix = "th";
 }
char pos[getNumberOfDigits(place)];
  sprintf(pos, "%i", place);

 char *returnable;

 returnable = (char *)malloc(getNumberOfDigits(place)+3);
for (int i = 0 ; i <getNumberOfDigits(place) ; i++ )
  {
returnable[i] = pos[i];
  }
returnable[getNumberOfDigits(place)] = suffix[0];

returnable[getNumberOfDigits(place)+1] = suffix[1];

returnable[getNumberOfDigits(place)+2] = '\0';

  return returnable;

}

unsigned getNumberOfDigits(unsigned i)
{
  return i > 0 ? (int)log10((double)i) + 1 : 1;
}

void changeSelectionScoreName(int letterNum, char *character, uint16_t colour)
{

  int textX = (0.5 * screenShort) - 24 + (letterNum * textWidth);

  TFTscreen.fillRect(textX, verticalDotCentrePosition + textHeight, textWidth, textHeight, Black);

  TFTscreen.stroke(colour);

  TFTscreen.text(character, textX, verticalDotCentrePosition + textHeight);
}

void makeSelectionScoreArrows()
{
  for (int i = 0; i < 8; i++)
  {
    changeSelectionScoreArrows(i, true, White);
    changeSelectionScoreArrows(i, false, White);
  }
}

void changeSelectionScoreArrows(int letterNum, bool up, uint16_t colour)
{

  int textX = (0.5 * screenShort) - 24 + (letterNum * textWidth);

  TFTscreen.stroke(colour);
  if (up)
  {

    for (int i = 0; i < 5; i++)
    {

      TFTscreen.point(textX + i, verticalDotCentrePosition + abs(i - 2));
    }
  }
  else
  {
    for (int i = 0; i < 5; i++)
    {

      TFTscreen.point(textX + i, verticalDotCentrePosition + (3 * textHeight) - abs(i - 2));
    }
  }
}

void textNameUnderscore(int letterNum, uint16_t colour)

{

  int textX = (0.5 * screenShort) - 24 + (letterNum * textWidth);

  TFTscreen.drawRect(textX, verticalDotCentrePosition + (2 * textHeight), textWidth, 2, colour);
}


void scoreBoard(bool trueScore)
{
  scoreFile = SD.open("scores.txt");
  TFTscreen.background(Black);
  TFTscreen.stroke(White);

  int textY = 5;
  int positionOnBoard = 1;
  structuredSelectableText("SCORES", textY, false, true, false);

  textY = textHeight * 3;

  //char malloc 90
  // save the number of registered new usernames
  // compare current to the set of 9 chars + (number of saved *9)
  // if they are the same, go onto the next, if they are never equal, register the new set of chars and print.

  char *bests;
  bests = (char *)malloc(90);
  int numberSaved = 0;
  while (scoreFile.available() && positionOnBoard < 11)
  {
    String string = scoreFile.readStringUntil(',');
    int scoreLineLength = string.length();

    char *nameInFile;
    nameInFile = (char *)malloc(9);
    for (int i = 0; i < 9; i++)
    {
      nameInFile[i] = string[i];
    }
    nameInFile[8] = '\0';

    char *scoreInFile;
    scoreInFile = (char *)malloc(scoreLineLength - 8);
    for (int i = 0; i < scoreLineLength - 8; i++)
    {
      scoreInFile[i] = string[i + 8];
    }
    scoreInFile[scoreLineLength - 8] = '\0';

    if (numberSaved == 0 || trueScore)
    {

      for (int j = 0; j < 9; j++)
      {
        bests[j] = nameInFile[j];
      }
      char pos[2];
      sprintf(pos, "%i", positionOnBoard);
      TFTscreen.text(pos, 0, textY);
      structuredSelectableText(nameInFile, textY, false, false, false);
      structuredSelectableText(scoreInFile, textY, false, false, true);

      textY = textY + textHeight + 5;
      positionOnBoard++;
      numberSaved++;
    }
    else
    {
      for (int i = 0; i < numberSaved; i++)
      {
        char *oneOfTheBest;
        oneOfTheBest = (char *)malloc(9);
        for (int j = 0; j < 8; j++)
        {
          oneOfTheBest[j] = bests[j + (i * 9)];
        }
        oneOfTheBest[8] = '\0';
        if (strcmp(nameInFile, oneOfTheBest) == 0)
        {
          i = numberSaved;
        }
        else if (strcmp(nameInFile, oneOfTheBest) != 0 && i == numberSaved - 1)
        {
          for (int j = 0; j < 9; j++)
          {
            bests[j + ((i + 1) * 9)] = nameInFile[j];
          }
          bests[9 + ((i + 1) * 9)] = '\0';

          char pos[2];
          sprintf(pos, "%i", positionOnBoard);
          TFTscreen.text(pos, 0, textY);
          structuredSelectableText(nameInFile, textY, false, false, false);
          structuredSelectableText(scoreInFile, textY, false, false, true);

          textY = textY + textHeight + 5;
          positionOnBoard++;
          numberSaved++;
          i = numberSaved;
        }
      }
    }

    free(nameInFile);
    free(scoreInFile);
  }

  scoreFile.close();

  waitForB();
  enterScores();
}

void enterScores()
{

  TFTscreen.background(Black);
  TFTscreen.stroke(White);

  int selected = scoreTypeSelection();

  switch (selected)
  {
  case 0:
    scoreBoard(true);
    break;
  case 1:
    scoreBoard(false);
    break;
  default:
    errorScreen("INVALID INT");
  }
}

int scoreTypeSelection()
{

  structuredSelectableText("SCORE MODE", verticalDotCentrePosition - (4 * textHeight), false, true, false);

  bool letGoOfUp = true;
  bool letGoOfDown = true;

  int selected = 0;
  changeSelectionScoreMenu(selected);

  while (true)
  {
    readButtonStates();

    if (downButtonState == HIGH && letGoOfDown)
    {
      if (selected < 1)
      {
        selected++;
        changeSelectionScoreMenu(selected);
      }
      letGoOfDown = false;
    }
    if (upButtonState == HIGH && letGoOfUp)
    {
      if (selected > 0)
      {
        selected--;
        changeSelectionScoreMenu(selected);
      }
      letGoOfUp = false;
    }

    if (aButtonState == HIGH)
    {
      return selected;
    }
    if (upButtonState == LOW)
    {
      letGoOfUp = true;
    }

    if (downButtonState == LOW)
    {
      letGoOfDown = true;
    }

    if (bButtonState == HIGH)
    {
      enterMenu();
    }

    delay(50);
  }
}

void changeSelectionScoreMenu(int selection)
{

  switch (selection)
  {
  case 0:

    structuredSelectableText("TRUE", verticalDotCentrePosition - textHeight, true, false, false);
    structuredSelectableText("PERSONAL BEST", verticalDotCentrePosition + textHeight, false, false, false);

    break;
  case 1:
    structuredSelectableText("TRUE", verticalDotCentrePosition - textHeight, false, false, false);
    structuredSelectableText("PERSONAL BEST", verticalDotCentrePosition + textHeight, true, false, false);

    break;

  default:
    errorScreen("INVALID INT");
  }
}

void structuredSelectableText(char *text, int height, bool selected, bool title, bool rightHandPushed)
{
 
  int multiplier = 1;
  if (title)
  {
    TFTscreen.setTextSize(2 * textMultiplier);
    multiplier = 2;
  }
  int textLength = multiplier * (textWidth * strlen(text));
  int textX = (screenShort - textLength);

  if (!rightHandPushed)
  {
    textX = textX / 2;
  }

  int16_t colourText;
  int16_t colourUnderscore;
  if (selected)
  {
    colourText = Green;
    colourUnderscore = Green;
  }
  else
  {
    colourText = White;
    colourUnderscore = Black;
  }
  TFTscreen.stroke(colourText);
  TFTscreen.text(text, textX, height);
  if (!title)
  {
    TFTscreen.drawRect(textX, height + (multiplier * textHeight), multiplier * textLength, 1, colourUnderscore);
  }

  TFTscreen.setTextSize(textMultiplier);
}

void waitForB()
{
  while (true)
  {
    bButtonState = digitalRead(B_BUTTON);

    if (bButtonState == HIGH)
    {
      return;
    }
  }
}

void enterSettings()
{

  TFTscreen.background(Yellow);
  TFTscreen.stroke(Black);
  TFTscreen.text("UNDER CONSTRUCTION :(", 0, 0);

  waitForB();
}

int menuSetup()
{
  TFTscreen.background(Black);
  structuredSelectableText("JETRIS", verticalDotCentrePosition - (4 * textHeight), false, true, false);

  bool letGoOfUp = true;
  bool letGoOfDown = true;

  int selected = 0;
  changeSelectionStartMenu(selected);

  while (true)
  {
    readButtonStates();

    if (downButtonState == HIGH && letGoOfDown)
    {
      if (selected < 2)
      {
        selected++;
        changeSelectionStartMenu(selected);
      }
      letGoOfDown = false;
    }
    if (upButtonState == HIGH && letGoOfUp)
    {
      if (selected > 0)
      {
        selected--;
        changeSelectionStartMenu(selected);
      }
      letGoOfUp = false;
    }

    if (aButtonState == HIGH)
    {
      return selected;
    }
    if (upButtonState == LOW)
    {
      letGoOfUp = true;
    }

    if (downButtonState == LOW)
    {
      letGoOfDown = true;
    }

    delay(50);
  }
}

void changeSelectionStartMenu(int selection)
{
  switch (selection)
  {
  case 0:

    structuredSelectableText("START", verticalDotCentrePosition - textHeight, true, false, false);
    structuredSelectableText("SCORES", verticalDotCentrePosition + textHeight, false, false, false);
    structuredSelectableText("SETTINGS", verticalDotCentrePosition + (3 * textHeight), false, false, false);

    break;
  case 1:
    structuredSelectableText("START", verticalDotCentrePosition - textHeight, false, false, false);
    structuredSelectableText("SCORES", verticalDotCentrePosition + textHeight, true, false, false);
    structuredSelectableText("SETTINGS", verticalDotCentrePosition + (3 * textHeight), false, false, false);

    break;
  case 2:
    structuredSelectableText("START", verticalDotCentrePosition - textHeight, false, false, false);
    structuredSelectableText("SCORES", verticalDotCentrePosition + textHeight, false, false, false);
    structuredSelectableText("SETTINGS", verticalDotCentrePosition + (3 * textHeight), true, false, false);
    break;
  default:
    errorScreen("INVALID INT");
  }
}

void errorScreen(char *message)
{
  TFTscreen.background(Red);
  TFTscreen.stroke(White);
  TFTscreen.text("OT OH :(", textHeight, 0);
  TFTscreen.text(message, 0, textHeight * 3);
  TFTscreen.text("PLEASE RESET", 0, textHeight * 5);

  while (true)
  {
  }
}

void gameScreenSetup()
{
  // clear the screen with a black background
  TFTscreen.background(Black);
  TFTscreen.stroke(White);
  //screen.rect(xStart, yStart, width, height);
  //xStart : int, the horizontal position where the line starts
  //yStart : int, the vertical position where the line starts
  //width : int, the width of the rectangle
  //height : int, the height of the rectangle
  TFTscreen.rect(horizontalPlayFieldStart, verticalPlayFieldStart, widthPlayFieldContainer, heightPlayFieldContainer);
  TFTscreen.setTextSize(textMultiplier);

  TFTscreen.rect(sideBoxHorizontal, holdBoxVertical, sideBoxUnitHorizontal, sideBoxUnitVertical);
  TFTscreen.text("HOLD", sideBoxHorizontal, verticalPlayFieldStart);

  TFTscreen.text("NEXT", sideBoxHorizontal, holdBoxVertical + sideBoxUnitVertical + multiplier);
  TFTscreen.rect(sideBoxHorizontal, nextUpVertical, sideBoxUnitHorizontal, 2 * sideBoxUnitVertical);

  TFTscreen.text("SCORE", sideBoxHorizontal, nextUpVertical + (2 * sideBoxUnitVertical) + multiplier);
  TFTscreen.text("0", sideBoxHorizontal, scoreTextVertical);

  TFTscreen.text("LEVEL", sideBoxHorizontal, nextUpVertical + (2 * sideBoxUnitVertical) + (4.5 * multiplier));
  TFTscreen.text("1", sideBoxHorizontal, levelTextVertical);
}

void gameOver()
{
  TFTscreen.fillRect(sideBoxHorizontal + lineDrawBuffer, nextUpVertical + lineDrawBuffer, sideBoxUnitHorizontal - (2 * lineDrawBuffer), (2 * sideBoxUnitVertical) - (2 * lineDrawBuffer), Black);
  TFTscreen.fillRect(sideBoxHorizontal + lineDrawBuffer, holdBoxVertical + lineDrawBuffer, sideBoxUnitHorizontal - (2 * lineDrawBuffer), sideBoxUnitVertical - (2 * lineDrawBuffer), Black);
  TFTscreen.fillRect(linePosHorizontalMaxLeft, linePosVerticalMaxUp, widthPlayField, heightPlayField, Black);
  TFTscreen.stroke(White);
  TFTscreen.text("GAME OVER", (middleOfPlayField - strlen("GAME OVER")) / 2, verticalDotCentrePosition);
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
  int lineMultiplier = 0;

  bool needsReDoing = false;
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

// spawns the terimino into the playgrid
void spawnTetrimino(Tetrimino tetrimino)
{
  long timeTaken;
  long previousTime;
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
      }
    }
  }

}

void moveTetrimino(Tetrimino tetrimino)
{
  readButtonStates();
  if (upButtonState == LOW)
  {
    letGoOfHardDrop = true;
  }

  if (downButtonState == HIGH && gameAlive && tetriminoAlive)
  {
    //TODO - implement legit faster fall rules
    tryToMoveDown(tetrimino, true);
  }
  if (upButtonState == HIGH && letGoOfHardDrop && gameAlive && tetriminoAlive)
  {
    hardDrop(tetrimino);
  }

  if (leftButtonState == HIGH && gameAlive && tetriminoAlive)
  {
    tryToMoveLeft(tetrimino);
  }
  if (rightButtonState == HIGH && gameAlive && tetriminoAlive)
  {
    tryToMoveRight(tetrimino);
  }

  if (aButtonState == HIGH && holdAvailable && gameAlive && tetriminoAlive)
  {
    tetriminoInPlay = false;
  }
  if (bButtonState == HIGH && letGoOfRotate && gameAlive && tetriminoAlive)
  {

    letGoOfRotate = false;
    tryToRotate(tetrimino);
  }
  if (bButtonState == LOW)
  {
    letGoOfRotate = true;
  }
  delay(50);
}

void hardDrop(Tetrimino tetrimino)
{
  int hypotheticalVerticalDotPosition = verticalDotPosition + multiplier;
  int counter = 0;
  while (!hitBottom(tetrimino, hypotheticalVerticalDotPosition) && !overlapOfPlayGrid(tetrimino, horizontalDotPosition, hypotheticalVerticalDotPosition))
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

void tryToMoveDown(Tetrimino tetrimino, bool incrementScore)
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

bool hitBottom(Tetrimino tetrimino, int hypotheticalVerticalDotPosition)
{
  if (hypotheticalVerticalDotPosition + (tetrimino.blocksDown() * multiplier) > linePosVerticalMaxDown)
  {
    return true;
  }
  return false;
}

bool overlapOfPlayGrid(Tetrimino tetrimino, int hypotheticalHorizontalDotPosition, int hypotheticalVerticalDotPosition)
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

  fillInGrid(tetrimino, previousHorizontalDotPosition, previousVerticalDotPosition, true, Black);
  fillInGrid(tetrimino, horizontalDotPosition, verticalDotPosition, true, tetrimino.colour);
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

void fillInGrid(Tetrimino tetrimino, int startHorizontalDotPosition, int startVerticalDotPosition, bool trueOrFalse, uint16_t colour)
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

      bool conditionalBoolean;
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
