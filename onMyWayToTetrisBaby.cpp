

//note to self: the scores still seem to be going funny- the commas aren't quite consistent.

#include <SD.h>
#include <TFT.h> // Arduino LCD library
#include <SPI.h>
#include <Arduino.h>
#include "tetrimino.h"
#define TFT_CS PB10
#define TFT_DC PB0
#define TFT_RST PB1

#define DOWN_BUTTON PA4
#define UP_BUTTON PA2
#define RIGHT_BUTTON PA6
#define LEFT_BUTTON PA3

#define B_BUTTON PA1
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

int multiplier;
int textMultiplier;

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
int verticalDotCentrePosition;
int horizontalDotCentrePosition;

int level;

// direction and speed
int speed;

int textHeight;
int textWidth;

int menuVerticalShift;

int startTextLength;
int startTextX;
int startTextY;
int scoreTextLength;
int scoreTextX;
int scoreTextY;
int settingsTextLength;
int settingsTextX;
int settingsTextY;

int scoreTrueTextLength;
int scoreTrueTextX;
int scoreBestTextLength;
int scoreBestTextX;
int scoreBestTextY;

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
bool tetriminoAlive;
bool tetriminoInPlay;
bool gameAlive;
bool letGoOfHardDrop;
bool letGoOfRotate;
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
void fillInGrid(Tetrimino tetrimino, int startHorizontalDotPosition, int startVerticalDotPosition, bool trueOrFalse, uint16_t colour);

void ghostGrid(Tetrimino tetrimino, int startHorizontalDotPosition, int startVerticalDotPosition, bool trueOrFalse, uint16_t colour);

void moveScreenTetrimino(Tetrimino tetrimino);
void rotateScreenTetrimino(Tetrimino tetrimino);
void movedDown(Tetrimino tetrimino);
double calculateFallSpeed(int level);
void movedRight(Tetrimino tetrimino);
void movedLeft(Tetrimino tetrimino);
void tryToRotate(Tetrimino tetrimino);
void tryToMoveDown(Tetrimino tetrimino, bool incrementScore);
void tryToMoveRight(Tetrimino tetrimino);
void tryToMoveLeft(Tetrimino tetrimino);
void ghostPositionUpdate(Tetrimino tetrimino);
bool hitBottom(Tetrimino tetrimino, int hypotheticalVerticalDotPosition);
bool overlapOfPlayGrid(Tetrimino tetrimino, int hypotheticalHorizontalDotPosition, int hypotheticalVerticalDotPosition);
void commitToPlayGrid(Tetrimino tetrimino);
void clearGrid();
void hardDrop(Tetrimino tetrimino);
void spawnGhost(Tetrimino tetrimino);
void ghostColour(Tetrimino tetrimino);
void updateGhost(Tetrimino tetrimino);
void updateGhostColour(Tetrimino tetrimino);
void checkForClearLine();
void randomSeven();
void checkForLevelUp();
void gameOver();
void gameScreenSetup();
int menuSetup();
void enterMenu();
char *newPlayer();
void enterGameEndedPhase();
void changeSelectionScoreArrows(int letterNum, bool up, uint16_t colour);
void makeSelectionScoreArrows();
void changeSelectionScoreName(int letterNum, char *character, uint16_t colour);
void textNameUnderscore(int letterNum, uint16_t colour);
int scoreTypeSelection();
void enterGame();
void enterScores();
void enterSettings();
void updateScore();
void updateLevel();
void changeSelectionStartMenu(int selection);
void changeSelectionScoreMenu(int selection);
void enterTrue();
bool emptyHold;
bool holdAvailable;
int selectorRand[7];
int score;

#define SD_MOSI PB15
#define SD_MISO PB14
#define SD_SCK PB13
#define SD_CS PB12

File scoreFile;

void setup()
{

  SerialUSB.begin();
  TFTscreen.begin();
  TFTscreen.setRotation(0);

  SD.begin(SD_CS, SD_MOSI, SD_MISO, SD_SCK);

  // PImage parrotImage = TFTscreen.loadImage("PARROT.BMP");
  // TFTscreen.image(parrotImage,0,0);
  //
  //   delay(5000);

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

  textHeight = textMultiplier * 8;
  textWidth = textMultiplier * 6;

  menuVerticalShift = verticalDotCentrePosition - (2 * textHeight);

  startTextLength = (textWidth * strlen("START"));
  startTextX = (screenShort - startTextLength) / 2;
  startTextY = menuVerticalShift + (textHeight);
  scoreTextLength = (textWidth * strlen("SCORES"));
  scoreTextX = (screenShort - scoreTextLength) / 2;
  scoreTextY = menuVerticalShift + (3 * textHeight);
  settingsTextLength = (textWidth * strlen("SETTINGS"));
  settingsTextX = (screenShort - settingsTextLength) / 2;
  settingsTextY = menuVerticalShift + (5 * textHeight);

  scoreTrueTextLength = (textWidth * strlen("TRUE"));
  scoreTrueTextX = (screenShort - scoreTrueTextLength) / 2;

  scoreBestTextLength = (textWidth * strlen("PERSONAL BEST"));
  scoreBestTextX = (screenShort - scoreBestTextLength) / 2;

  sideBoxUnitHorizontal = (4 * multiplier) + (2 * lineDrawBuffer);
  sideBoxUnitVertical = (3 * multiplier) + (2 * lineDrawBuffer);
  sideBoxHorizontal = ((screenShort + (widthPlayFieldContainer + horizontalPlayFieldStart) - sideBoxUnitHorizontal) / 2) - lineDrawBuffer;
  holdBoxVertical = verticalPlayFieldStart + multiplier;

  nextUpVertical = holdBoxVertical + sideBoxUnitVertical + (2 * multiplier);
  scoreTextVertical = nextUpVertical + (2 * sideBoxUnitVertical) + (2.5 * multiplier);
  levelTextVertical = nextUpVertical + (2 * sideBoxUnitVertical) + (6 * multiplier);

  TFTscreen.setTextSize(textMultiplier);
  // SD.remove("scores.txt");
  scoreFile = SD.open("scores.txt", FILE_WRITE);
  scoreFile.close();
  if (!SD.exists("scores.txt"))
  {
    TFTscreen.background(Red);
    TFTscreen.stroke(White);
    TFTscreen.text("SCORES FILE BROKEN :(", 0, 0);
    while(true){

    }
  }
}

void loop()
{
  enterMenu();
}

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
    TFTscreen.background(Yellow);
    TFTscreen.stroke(Black);
    TFTscreen.text("CRIES IN IMPOSSSIBRU :(", 0, 0);
    delay(5000);
  }
}

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

char *newPlayer()
{
  char *characters[27] = {" ", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"};
  int letters[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  int charactSelec = 0;
  makeSelectionScoreArrows();
  bool letGoOfA = true;
  bool letGoOfB = true;

  textNameUnderscore(charactSelec, Green);

  while (true)
  {
    downButtonState = digitalRead(DOWN_BUTTON);
    upButtonState = digitalRead(UP_BUTTON);
    aButtonState = digitalRead(A_BUTTON);
    bButtonState = digitalRead(B_BUTTON);

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

    delay(75);
  }
  TFTscreen.background(Red);
  delay(1000);

  char *name;
  name = (char *)malloc(9);
  for (int i = 0; i < 8; i++)
  {
    name[i] = *characters[letters[i]];
    SerialUSB.print(name[i]);
  }
  SerialUSB.println("!");
  name[8] = '\0';

  SerialUSB.print("Returning name ");
  SerialUSB.print(name);
  SerialUSB.println("!");
  return name;
}

void enterGameEndedPhase()
{

  // Here, what I want to do is, I want to access the database of saved people and their scores.
  //Step one, select player or create new player.

  TFTscreen.background(Black);
  char *name = newPlayer();
  SerialUSB.print("Player name ");
  SerialUSB.print(name);

  SerialUSB.print(" got ");
  SerialUSB.println(score);

  // If select player, display the names with arrow keys, A selects the name, B goes back to the previous screen.
  // Once selected, display their highscore and current score.
  // If the current score beats the high score, some kind of congrats and then overwrite the previous.
  // If it does not, say, better luck next timen [name].

  // If creating a new player, simply add their highscore and name.
  // Welcome, [name]. Your highscore has now been saved.

  // Here's where you rank amongst others:
  // a reel of the scores and their usernames
  // Some kind of ordering will have to happen here with the highscores (with the current person highlighted or in a different colour (green?)).

  // I should save them in order so that the highscore accessed from the menu is just a matter of reading the text file and displaying.
  // Clicking next displays a prophecy:
  // no one has yet to beat the mighty [name]

  // SerialUSB.println("checking file...");
  int scoreLinesCounting = 0;
  String restOfFile = "";

  scoreFile = SD.open("scores.txt");

  while (scoreFile.available())
  {
    SerialUSB.println("reading line...");
    String string = scoreFile.readStringUntil(',');
    SerialUSB.println(string);
    int scoreLineLength = string.length();
    SerialUSB.println("reading name...");
    char *nameInBoard;
    nameInBoard = (char *)malloc(9);
    for (int i = 0; i < 8; i++)
    {
      nameInBoard[i] = string[i];
      SerialUSB.print(string[i]);
    }
    SerialUSB.print(".");
    SerialUSB.println();
    nameInBoard[8] = '\0';

    SerialUSB.println("reading score...");
    int actualIntScore = 0;

    for (int i = 0; i < scoreLineLength - 8; i++)
    {
      int tenthPow = 1;
      for (int j = 1; j < (scoreLineLength - 8 - i); j++)
      {
        tenthPow = tenthPow * 10;
      }

      actualIntScore = actualIntScore + (tenthPow * (string[i + 8] - '0'));
      SerialUSB.print(string[i + 8]);
    }

    SerialUSB.println();

    SerialUSB.print(nameInBoard);
    SerialUSB.print(" got a score of ");
    SerialUSB.println(actualIntScore);

    SerialUSB.print("I got a score of ");
    SerialUSB.println(score);

    if (score <= actualIntScore)
    {
      scoreLinesCounting =
          scoreLinesCounting + scoreLineLength + 1;
    }
    else
    {
      restOfFile = string + ',' + scoreFile.readString();

      SerialUSB.println("the rest of the file: ");
      SerialUSB.println(restOfFile);
      break;
    }

    free(nameInBoard);
  }

  scoreFile.close();
   SerialUSB.println("appending to file...");
  scoreFile = SD.open("scores.txt", FILE_WRITE);
  scoreFile.seek(scoreLinesCounting);
  scoreFile.print(name);
  scoreFile.print(score);
  scoreFile.print(",");
  scoreFile.print(restOfFile);
  scoreFile.close();

  SerialUSB.println("done!");

  free(name);
  SerialUSB.println();
}

void changeSelectionScoreName(int letterNum, char *character, uint16_t colour)
{

  int textX = (0.5 * screenShort) - 24 + (letterNum * textWidth);

  TFTscreen.fillRect(textX, scoreTextY, textWidth, textHeight, Black);

  TFTscreen.stroke(colour);

  TFTscreen.text(character, textX, scoreTextY);
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

      TFTscreen.point(textX + i, scoreTextY - textHeight + abs(i - 2));
    }
  }
  else
  {
    for (int i = 0; i < 5; i++)
    {

      TFTscreen.point(textX + i, scoreTextY + (2 * textHeight) - abs(i - 2));
    }
  }
}

void textNameUnderscore(int letterNum, uint16_t colour)



{

  int textX = (0.5 * screenShort) - 24 + (letterNum * textWidth);

  TFTscreen.drawRect(textX, scoreTextY + textHeight, textWidth, 2, colour);
}


void enterTrue(){
  scoreFile = SD.open("scores.txt");
TFTscreen.background(Black);
TFTscreen.stroke(White);

int textY = 5;

  while (scoreFile.available())
  {
    
    String string = scoreFile.readStringUntil(',');

    int n = string.length(); 
    char char_array[n + 1]; 
  
    // copying the contents of the 
    // string to char array 
    strcpy(char_array, string.c_str());

    

        TFTscreen.text(char_array, 5, textY );

    textY = textY + textHeight + 5;
  }

  scoreFile.close();

}

void enterScores()
{
  TFTscreen.background(Black);
  TFTscreen.stroke(White);

  int selected = scoreTypeSelection();

  switch (selected)
  {
  case 0:
    enterTrue();
      while (true)
  {
    bButtonState = digitalRead(B_BUTTON);

    if (bButtonState == HIGH)
    {
      return;
    }
  }
    // TFTscreen.background(Yellow);
    // TFTscreen.stroke(Black);
    // TFTscreen.text("TRUE", 0, 0);
    // delay(2000);
    break;
  case 1:
    // enterPersonalBest();
    TFTscreen.background(Yellow);
    TFTscreen.stroke(Black);
    TFTscreen.text("PERSONAL BEST", 0, 0);
    delay(2000);
    break;
  default:
    TFTscreen.background(Yellow);
    TFTscreen.stroke(Black);
    TFTscreen.text("CRIES IN IMPOSSSIBRU :(", 0, 0);
    delay(5000);
  }

  while (true)
  {
    bButtonState = digitalRead(B_BUTTON);

    if (bButtonState == HIGH)
    {
      return;
    }
  }
}

int scoreTypeSelection()
{

  TFTscreen.setTextSize(2 * textMultiplier);
  size_t scoreModeStringLength = strlen("SCORE MODE");

  TFTscreen.text("SCORE MODE", (screenShort - (2 * textWidth * scoreModeStringLength)) / 2, menuVerticalShift - (2 * textHeight));
  TFTscreen.setTextSize(textMultiplier);

  bool letGoOfUp = true;
  bool letGoOfDown = true;

  int selected = 0;
  changeSelectionScoreMenu(selected);

  while (true)
  {
    downButtonState = digitalRead(DOWN_BUTTON);
    upButtonState = digitalRead(UP_BUTTON);
    aButtonState = digitalRead(A_BUTTON);

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

    delay(50);
  }
}

void changeSelectionScoreMenu(int selection)
{

  switch (selection)
  {
  case 0:
    TFTscreen.stroke(Green);
    TFTscreen.text("TRUE", scoreTrueTextX, startTextY);
    TFTscreen.drawRect(scoreTrueTextX, startTextY + textHeight, scoreTrueTextLength, 1, Green);

    TFTscreen.stroke(White);
    TFTscreen.text("PERSONAL BEST", scoreBestTextX, scoreTextY);
    TFTscreen.drawRect(scoreBestTextX, scoreTextY + textHeight, scoreBestTextLength, 1, Black);

    break;
  case 1:
    TFTscreen.stroke(Green);
    TFTscreen.text("PERSONAL BEST", scoreBestTextX, scoreTextY);
    TFTscreen.drawRect(scoreBestTextX, scoreTextY + textHeight, scoreBestTextLength, 1, Green);

    TFTscreen.stroke(White);
    TFTscreen.text("TRUE", scoreTrueTextX, startTextY);
    TFTscreen.drawRect(scoreTrueTextX, startTextY + textHeight, scoreTrueTextLength, 1, Black);

    break;

  default:
    TFTscreen.background(Red);
    TFTscreen.stroke(White);
    TFTscreen.text("OT OH :(", 0, 0);
    delay(5000);
    break;
  }
}

void enterSettings()
{

  TFTscreen.background(Yellow);
  TFTscreen.stroke(Black);
  TFTscreen.text("UNDER CONSTRUCTION :(", 0, 0);

  while (true)
  {
    bButtonState = digitalRead(B_BUTTON);

    if (bButtonState == HIGH)
    {
      return;
    }
  }
}

int menuSetup()
{
  TFTscreen.background(Black);
  TFTscreen.stroke(White);

  TFTscreen.setTextSize(2 * textMultiplier);
  size_t jetrisStringLength = strlen("JETRIS");

  TFTscreen.text("JETRIS", (screenShort - (2 * textWidth * jetrisStringLength)) / 2, menuVerticalShift - (2 * textHeight));
  TFTscreen.setTextSize(textMultiplier);

  bool letGoOfUp = true;
  bool letGoOfDown = true;

  int selected = 0;
  changeSelectionStartMenu(selected);

  while (true)
  {
    downButtonState = digitalRead(DOWN_BUTTON);
    upButtonState = digitalRead(UP_BUTTON);
    aButtonState = digitalRead(A_BUTTON);

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
    TFTscreen.stroke(Green);
    TFTscreen.text("START", startTextX, startTextY);
    TFTscreen.drawRect(startTextX, startTextY + textHeight, startTextLength, 1, Green);

    TFTscreen.stroke(White);

    TFTscreen.text("SCORES", scoreTextX, scoreTextY);

    TFTscreen.drawRect(scoreTextX, scoreTextY + textHeight, scoreTextLength, 1, Black);

    TFTscreen.stroke(White);
    TFTscreen.text("SETTINGS", settingsTextX, settingsTextY);
    TFTscreen.drawRect(settingsTextX, settingsTextY + textHeight, settingsTextLength, 1, Black);

    break;
  case 1:
    TFTscreen.stroke(Green);

    TFTscreen.text("SCORES", scoreTextX, scoreTextY);

    TFTscreen.drawRect(scoreTextX, scoreTextY + textHeight, scoreTextLength, 1, Green);

    TFTscreen.stroke(White);
    TFTscreen.text("START", startTextX, startTextY);
    TFTscreen.drawRect(startTextX, startTextY + textHeight, startTextLength, 1, Black);

    TFTscreen.stroke(White);
    TFTscreen.text("SETTINGS", settingsTextX, settingsTextY);
    TFTscreen.drawRect(settingsTextX, settingsTextY + textHeight, settingsTextLength, 1, Black);

    break;
  case 2:
    TFTscreen.stroke(Green);

    TFTscreen.text("SETTINGS", settingsTextX, settingsTextY);
    TFTscreen.drawRect(settingsTextX, settingsTextY + textHeight, settingsTextLength, 1, Green);

    TFTscreen.stroke(White);
    TFTscreen.text("START", startTextX, startTextY);
    TFTscreen.drawRect(startTextX, startTextY + textHeight, startTextLength, 1, Black);

    TFTscreen.stroke(White);

    TFTscreen.text("SCORES", scoreTextX, scoreTextY);

    TFTscreen.drawRect(scoreTextX, scoreTextY + textHeight, scoreTextLength, 1, Black);
    break;
  default:
    TFTscreen.background(Red);
    TFTscreen.stroke(White);
    TFTscreen.text("OT OH :(", 0, 0);
    delay(5000);
    break;
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
  lineMultiplier = 0;

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

  if (downButtonState == HIGH && gameAlive)
  {
    //TODO - implement legit faster fall rules
    tryToMoveDown(tetrimino, true);
  }
  if (upButtonState == HIGH && letGoOfHardDrop && gameAlive)
  {
    hardDrop(tetrimino);
  }
  if (leftButtonState == HIGH && gameAlive)
  {
    tryToMoveLeft(tetrimino);
  }
  if (rightButtonState == HIGH && gameAlive)
  {

    tryToMoveRight(tetrimino);
  }

  if (aButtonState == HIGH && holdAvailable && gameAlive)
  {
    tetriminoInPlay = false;
  }
  if (bButtonState == HIGH && letGoOfRotate && gameAlive)
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
