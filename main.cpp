#include "main.h"

// the level of the player (to determine fall speed)
int level;

// direction and speed
int speed;

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

// the tetrimino in the hold cell
Tetrimino holdTetrimino;

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

SillyGoose sillyGoose;

TetriminoCollection tetriminoCollection;

File scoreFile;

void setup()
{
  sillyGoose.initialise(tetrisGridRows);
  tetriminoCollection.initialise(sillyGoose.scaleDisplay);
 
  SerialUSB.begin();

  heightPlayField = (tetrisGridRows * sillyGoose.scaleDisplay);
  heightPlayFieldContainer = heightPlayField + (2 * lineDrawBuffer);
  widthPlayField = (tetrisGridCols * sillyGoose.scaleDisplay);
  widthPlayFieldContainer = widthPlayField + (2 * lineDrawBuffer);

  verticalPlayFieldStart = sillyGoose.verticalDotCentrePosition - (heightPlayFieldContainer / 2);
  horizontalPlayFieldStart = verticalPlayFieldStart;
  linePosVerticalMaxUp = verticalPlayFieldStart + lineDrawBuffer;
  linePosVerticalMaxDown = linePosVerticalMaxUp + heightPlayField;
  linePosHorizontalMaxLeft = horizontalPlayFieldStart + lineDrawBuffer;
  linePosHorizontalMaxRight = linePosHorizontalMaxLeft + widthPlayField;

  verticalStartPosition = linePosVerticalMaxUp - (numInvisableRows * sillyGoose.scaleDisplay);

  middleOfPlayField = linePosHorizontalMaxLeft + (widthPlayField / 2);
  horizontalStartPosition = middleOfPlayField - sillyGoose.scaleDisplay;

  // variables to keep track of the point's location
  verticalDotPosition = verticalStartPosition;
  horizontalDotPosition = horizontalStartPosition;
  previousVerticalDotPosition = verticalStartPosition;
  previousHorizontalDotPosition = horizontalStartPosition;

  verticalGhostPosition = verticalStartPosition;
  previousVerticalGhostPosition = verticalStartPosition;

  sideBoxUnitHorizontal = (4 * sillyGoose.scaleDisplay) + (2 * lineDrawBuffer);
  sideBoxUnitVertical = (3 * sillyGoose.scaleDisplay) + (2 * lineDrawBuffer);
  sideBoxHorizontal = ((sillyGoose.screenShort + (widthPlayFieldContainer + horizontalPlayFieldStart) - sideBoxUnitHorizontal) / 2) - lineDrawBuffer;
  holdBoxVertical = verticalPlayFieldStart + sillyGoose.scaleDisplay;

  nextUpVertical = holdBoxVertical + sideBoxUnitVertical + (2 * sillyGoose.scaleDisplay);
  scoreTextVertical = nextUpVertical + (2 * sideBoxUnitVertical) + (2.5 * sillyGoose.scaleDisplay);
  levelTextVertical = nextUpVertical + (2 * sideBoxUnitVertical) + (6 * sillyGoose.scaleDisplay);

  
  scoreFile = SD.open("scores.txt", FILE_WRITE);
  scoreFile.close();
  if (!SD.exists("scores.txt"))
  {
    sillyGoose.errorScreen("SCORES FILE BROKEN");
  }
}

void loop()
{
  enterMenu();
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
    sillyGoose.errorScreen("INVALID INT");
  }
}

// Begins the tetris gameplay
void enterGame()
{
  gameScreenSetup();

  sillyGoose.TFTscreen.fillRect(linePosHorizontalMaxLeft, linePosVerticalMaxUp, widthPlayField, heightPlayField, Black);
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

  std::vector<Tetrimino> randomTetriminos;
  std::vector<Tetrimino> randomTetriminosSet = tetriminoCollection.getRandomSet();
  for (int index = 0; index < 7; index++)
  {
    randomTetriminos.push_back( randomTetriminosSet[index]);
  }

  randomTetriminosSet = tetriminoCollection.getRandomSet();
  for (int index = 0; index < 7; index++)
  {
     randomTetriminos.push_back( randomTetriminosSet[index]);
  }

  while (gameAlive)
  {

    for (int index = 0; index < 7; index++)
    {
      sillyGoose.TFTscreen.fillRect(sideBoxHorizontal + lineDrawBuffer, nextUpVertical + lineDrawBuffer, sideBoxUnitHorizontal - (2 * lineDrawBuffer), (2 * sideBoxUnitVertical) - (2 * lineDrawBuffer), Black);

      Tetrimino nextTet = randomTetriminos[index + 1];
      fillInGrid(nextTet, sideBoxHorizontal + lineDrawBuffer, nextUpVertical + lineDrawBuffer, true, nextTet.colour);
      Tetrimino nextNextTet = randomTetriminos[index + 2];
      fillInGrid(nextNextTet, sideBoxHorizontal + lineDrawBuffer, nextUpVertical + sideBoxUnitVertical + lineDrawBuffer, true, nextNextTet.colour);
      spawnTetrimino(randomTetriminos[index]);

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
  randomTetriminosSet = tetriminoCollection.getRandomSet();
  for (int index = 0; index < 7; index++)
  {
    randomTetriminos[index + 7] = randomTetriminosSet[index];
  }
  }
  gameOver();
  delay(1000);
  enterGameEndedPhase();
}

// Allows the player to create a name to save to the scoreboard.
char *newPlayer()
{
    sillyGoose.TFTscreen.background(Black);
  sillyGoose.structuredSelectableText("ENTER", sillyGoose.verticalDotCentrePosition - (7 * sillyGoose.textHeight), false, true, false);
  sillyGoose.structuredSelectableText("YOUR", sillyGoose.verticalDotCentrePosition - (5 * sillyGoose.textHeight), false, true, false);
  sillyGoose.structuredSelectableText("NAME", sillyGoose.verticalDotCentrePosition - (3 * sillyGoose.textHeight), false, true, false);
  char *characters[27] = {" ", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"};
  int letters[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  int charactSelec = 0;
  makeSelectionScoreArrows();
  bool letGoOfA = true;
  bool letGoOfB = true;

  textNameUnderscore(charactSelec, Green);

  while (true)
  {
   sillyGoose. readButtonStates();

    if (sillyGoose.downButtonState == HIGH)
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
    if (sillyGoose.upButtonState == HIGH)
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
    if (sillyGoose.upButtonState == LOW)
    {
      changeSelectionScoreArrows(charactSelec, true, White);
    }
    if (sillyGoose.downButtonState == HIGH)
    {
      changeSelectionScoreArrows(charactSelec, false, White);
    }

    if (sillyGoose.aButtonState == HIGH && letGoOfA)
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

    if (sillyGoose.bButtonState == HIGH && letGoOfB)
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
    if (sillyGoose.aButtonState == LOW)
    {
      letGoOfA = true;
    }
    if (sillyGoose.bButtonState == LOW)
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

  sillyGoose.TFTscreen.background(Black);
  char *name = newPlayer();
  sillyGoose.TFTscreen.background(Black);
  sillyGoose.structuredSelectableText("PLEASE", sillyGoose.verticalDotCentrePosition - (2 * sillyGoose.textHeight), false, true, false);
  sillyGoose.structuredSelectableText("WAIT", sillyGoose.verticalDotCentrePosition, false, true, false);

  int scoreLinesCounting = 0;
  String restOfFile = "";
  int place = 1;
  scoreFile = SD.open("scores.txt");

  while (scoreFile.available() && place < 1001)
  {
    int i=10;
    if(i=20){

    }
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
  sillyGoose.TFTscreen.background(Black);

  sillyGoose.structuredSelectableText("YOU ARE", sillyGoose.verticalDotCentrePosition - (3 * sillyGoose.textHeight), false, true, false);

  char *positionText = position(place);
  sillyGoose.structuredSelectableText(positionText, sillyGoose.verticalDotCentrePosition, false, true, false);
 free(positionText);
  free(name);

  sillyGoose.structuredSelectableText("PRESS B", sillyGoose.verticalDotCentrePosition + (6 * sillyGoose.textHeight), false, false, false);
  sillyGoose.structuredSelectableText("FOR SCORES", sillyGoose.verticalDotCentrePosition + (7 * sillyGoose.textHeight), false, false, false);
  sillyGoose.waitForB();
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

  int textX = (0.5 * sillyGoose.screenShort) - 24 + (letterNum * sillyGoose.textWidth);

  sillyGoose.TFTscreen.fillRect(textX, sillyGoose.verticalDotCentrePosition + sillyGoose.textHeight, sillyGoose.textWidth, sillyGoose.textHeight, Black);

  sillyGoose.TFTscreen.stroke(colour);

  sillyGoose.TFTscreen.text(character, textX, sillyGoose.verticalDotCentrePosition + sillyGoose.textHeight);
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

  int textX = (0.5 * sillyGoose.screenShort) - 24 + (letterNum * sillyGoose.textWidth);

  sillyGoose.TFTscreen.stroke(colour);
  if (up)
  {

    for (int i = 0; i < 5; i++)
    {

      sillyGoose.TFTscreen.point(textX + i, sillyGoose.verticalDotCentrePosition + abs(i - 2));
    }
  }
  else
  {
    for (int i = 0; i < 5; i++)
    {

      sillyGoose.TFTscreen.point(textX + i, sillyGoose.verticalDotCentrePosition + (3 * sillyGoose.textHeight) - abs(i - 2));
    }
  }
}

void textNameUnderscore(int letterNum, uint16_t colour)

{

  int textX = (0.5 * sillyGoose.screenShort) - 24 + (letterNum * sillyGoose.textWidth);

  sillyGoose.TFTscreen.drawRect(textX, sillyGoose.verticalDotCentrePosition + (2 * sillyGoose.textHeight), sillyGoose.textWidth, 2, colour);
}


void scoreBoard(bool trueScore)
{
  scoreFile = SD.open("scores.txt");
  sillyGoose.TFTscreen.background(Black);
  sillyGoose.TFTscreen.stroke(White);

  int textY = 5;
  int positionOnBoard = 1;
  sillyGoose.structuredSelectableText("SCORES", textY, false, true, false);

  textY = sillyGoose.textHeight * 3;

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
      sillyGoose.TFTscreen.text(pos, 0, textY);
      sillyGoose.structuredSelectableText(nameInFile, textY, false, false, false);
      sillyGoose.structuredSelectableText(scoreInFile, textY, false, false, true);

      textY = textY + sillyGoose.textHeight + 5;
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
          sillyGoose.TFTscreen.text(pos, 0, textY);
          sillyGoose.structuredSelectableText(nameInFile, textY, false, false, false);
          sillyGoose.structuredSelectableText(scoreInFile, textY, false, false, true);

          textY = textY + sillyGoose.textHeight + 5;
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

  sillyGoose.waitForB();
  enterScores();
}

void enterScores()
{

  sillyGoose.TFTscreen.background(Black);
  sillyGoose.TFTscreen.stroke(White);

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
    sillyGoose.errorScreen("INVALID INT");
  }
}

int scoreTypeSelection()
{

  sillyGoose.structuredSelectableText("SCORE MODE", sillyGoose.verticalDotCentrePosition - (4 * sillyGoose.textHeight), false, true, false);

  bool letGoOfUp = true;
  bool letGoOfDown = true;

  int selected = 0;
  changeSelectionScoreMenu(selected);

  while (true)
  {
    sillyGoose.readButtonStates();

    if (sillyGoose.downButtonState == HIGH && letGoOfDown)
    {
      if (selected < 1)
      {
        selected++;
        changeSelectionScoreMenu(selected);
      }
      letGoOfDown = false;
    }
    if (sillyGoose.upButtonState == HIGH && letGoOfUp)
    {
      if (selected > 0)
      {
        selected--;
        changeSelectionScoreMenu(selected);
      }
      letGoOfUp = false;
    }

    if (sillyGoose.aButtonState == HIGH)
    {
      return selected;
    }
    if (sillyGoose.upButtonState == LOW)
    {
      letGoOfUp = true;
    }

    if (sillyGoose.downButtonState == LOW)
    {
      letGoOfDown = true;
    }

    if (sillyGoose.bButtonState == HIGH)
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

    sillyGoose.structuredSelectableText("TRUE", sillyGoose.verticalDotCentrePosition - sillyGoose.textHeight, true, false, false);
    sillyGoose.structuredSelectableText("PERSONAL BEST", sillyGoose.verticalDotCentrePosition + sillyGoose.textHeight, false, false, false);

    break;
  case 1:
    sillyGoose.structuredSelectableText("TRUE", sillyGoose.verticalDotCentrePosition - sillyGoose.textHeight, false, false, false);
    sillyGoose.structuredSelectableText("PERSONAL BEST", sillyGoose.verticalDotCentrePosition + sillyGoose.textHeight, true, false, false);

    break;

  default:
    sillyGoose.errorScreen("INVALID INT");
  }
}


void enterSettings()
{

  sillyGoose.TFTscreen.background(Yellow);
  sillyGoose.TFTscreen.stroke(Black);
  sillyGoose.TFTscreen.text("UNDER CONSTRUCTION :(", 0, 0);

  sillyGoose.waitForB();
}

int menuSetup()
{
  sillyGoose.TFTscreen.background(Black);
  sillyGoose.structuredSelectableText("JETRIS", sillyGoose.verticalDotCentrePosition - (4 * sillyGoose.textHeight), false, true, false);

  bool letGoOfUp = true;
  bool letGoOfDown = true;

  int selected = 0;
  changeSelectionStartMenu(selected);

  while (true)
  {
    sillyGoose.readButtonStates();

    if (sillyGoose.downButtonState == HIGH && letGoOfDown)
    {
      if (selected < 2)
      {
        selected++;
        changeSelectionStartMenu(selected);
      }
      letGoOfDown = false;
    }
    if (sillyGoose.upButtonState == HIGH && letGoOfUp)
    {
      if (selected > 0)
      {
        selected--;
        changeSelectionStartMenu(selected);
      }
      letGoOfUp = false;
    }

    if (sillyGoose.aButtonState == HIGH)
    {
      return selected;
    }
    if (sillyGoose.upButtonState == LOW)
    {
      letGoOfUp = true;
    }

    if (sillyGoose.downButtonState == LOW)
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

    sillyGoose.structuredSelectableText("START", sillyGoose.verticalDotCentrePosition - sillyGoose.textHeight, true, false, false);
    sillyGoose.structuredSelectableText("SCORES", sillyGoose.verticalDotCentrePosition + sillyGoose.textHeight, false, false, false);
    sillyGoose.structuredSelectableText("SETTINGS", sillyGoose.verticalDotCentrePosition + (3 * sillyGoose.textHeight), false, false, false);

    break;
  case 1:
    sillyGoose.structuredSelectableText("START", sillyGoose.verticalDotCentrePosition - sillyGoose.textHeight, false, false, false);
    sillyGoose.structuredSelectableText("SCORES", sillyGoose.verticalDotCentrePosition + sillyGoose.textHeight, true, false, false);
    sillyGoose.structuredSelectableText("SETTINGS", sillyGoose.verticalDotCentrePosition + (3 * sillyGoose.textHeight), false, false, false);

    break;
  case 2:
    sillyGoose.structuredSelectableText("START", sillyGoose.verticalDotCentrePosition - sillyGoose.textHeight, false, false, false);
    sillyGoose.structuredSelectableText("SCORES", sillyGoose.verticalDotCentrePosition + sillyGoose.textHeight, false, false, false);
    sillyGoose.structuredSelectableText("SETTINGS", sillyGoose.verticalDotCentrePosition + (3 * sillyGoose.textHeight), true, false, false);
    break;
  default:
    sillyGoose.errorScreen("INVALID INT");
  }
}



void gameScreenSetup()
{
  // clear the screen with a black background
  sillyGoose.TFTscreen.background(Black);
  sillyGoose.TFTscreen.stroke(White);
  //screen.rect(xStart, yStart, width, height);
  //xStart : int, the horizontal position where the line starts
  //yStart : int, the vertical position where the line starts
  //width : int, the width of the rectangle
  //height : int, the height of the rectangle
  sillyGoose.TFTscreen.rect(horizontalPlayFieldStart, verticalPlayFieldStart, widthPlayFieldContainer, heightPlayFieldContainer);
  sillyGoose.TFTscreen.setTextSize(sillyGoose.scaleText);

  sillyGoose.TFTscreen.rect(sideBoxHorizontal, holdBoxVertical, sideBoxUnitHorizontal, sideBoxUnitVertical);
  sillyGoose.TFTscreen.text("HOLD", sideBoxHorizontal, verticalPlayFieldStart);

  sillyGoose.TFTscreen.text("NEXT", sideBoxHorizontal, holdBoxVertical + sideBoxUnitVertical + sillyGoose.scaleDisplay);
  sillyGoose.TFTscreen.rect(sideBoxHorizontal, nextUpVertical, sideBoxUnitHorizontal, 2 * sideBoxUnitVertical);

  sillyGoose.TFTscreen.text("SCORE", sideBoxHorizontal, nextUpVertical + (2 * sideBoxUnitVertical) + sillyGoose.scaleDisplay);
  sillyGoose.TFTscreen.text("0", sideBoxHorizontal, scoreTextVertical);

  sillyGoose.TFTscreen.text("LEVEL", sideBoxHorizontal, nextUpVertical + (2 * sideBoxUnitVertical) + (4.5 * sillyGoose.scaleDisplay));
  sillyGoose.TFTscreen.text("1", sideBoxHorizontal, levelTextVertical);
}

void gameOver()
{
  sillyGoose.TFTscreen.fillRect(sideBoxHorizontal + lineDrawBuffer, nextUpVertical + lineDrawBuffer, sideBoxUnitHorizontal - (2 * lineDrawBuffer), (2 * sideBoxUnitVertical) - (2 * lineDrawBuffer), Black);
  sillyGoose.TFTscreen.fillRect(sideBoxHorizontal + lineDrawBuffer, holdBoxVertical + lineDrawBuffer, sideBoxUnitHorizontal - (2 * lineDrawBuffer), sideBoxUnitVertical - (2 * lineDrawBuffer), Black);
  sillyGoose.TFTscreen.fillRect(linePosHorizontalMaxLeft, linePosVerticalMaxUp, widthPlayField, heightPlayField, Black);
  sillyGoose.TFTscreen.stroke(White);
  sillyGoose.TFTscreen.text("GAME OVER", (middleOfPlayField - strlen("GAME OVER")) / 2, sillyGoose.verticalDotCentrePosition);
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
  sillyGoose.TFTscreen.fillRect(sideBoxHorizontal, scoreTextVertical, sideBoxUnitHorizontal, sillyGoose.scaleDisplay, Black);
  sillyGoose.TFTscreen.stroke(White);
  std::string str = std::to_string(score);
  const char *cstr2 = str.c_str();
  sillyGoose.TFTscreen.text(cstr2, sideBoxHorizontal, scoreTextVertical);
}

void updateLevel()
{
  sillyGoose.TFTscreen.fillRect(sideBoxHorizontal, levelTextVertical, sideBoxUnitHorizontal, sillyGoose.scaleDisplay, Black);
  sillyGoose.TFTscreen.stroke(White);
  std::string str = std::to_string(level);
  const char *cstr2 = str.c_str();
  sillyGoose.TFTscreen.text(cstr2, sideBoxHorizontal, levelTextVertical);
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
    sillyGoose.TFTscreen.fillRect(linePosHorizontalMaxLeft, linePosVerticalMaxUp, widthPlayField, heightPlayField, Black);

    for (int u = 0; u < tetrisGridRowsIncInvis; u++)
    {
      for (int r = 0; r < tetrisGridCols; r++)
      {

        uint16_t colourOfGrid = playGrid[r + (tetrisGridCols * u)];
        if (colourOfGrid != 0)
        {
          int rationalHorizontal = ((r * sillyGoose.scaleDisplay) + linePosHorizontalMaxLeft + 1);

          int rationalVertical = ((u - numInvisableRows) * sillyGoose.scaleDisplay) + linePosVerticalMaxUp + 1;

          sillyGoose.TFTscreen.fillRect(rationalHorizontal, rationalVertical, sillyGoose.scaleDisplay - 1, sillyGoose.scaleDisplay - 1, colourOfGrid);
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
    sillyGoose.TFTscreen.fillRect(sideBoxHorizontal + lineDrawBuffer, holdBoxVertical + lineDrawBuffer, sideBoxUnitHorizontal - (2 * lineDrawBuffer), sideBoxUnitVertical - (2 * lineDrawBuffer), Black);
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
        int rationalHorizontal = (horizontalDotPosition - linePosHorizontalMaxLeft) / sillyGoose.scaleDisplay;
        int rationalVertical = numInvisableRows + ((verticalDotPosition - linePosVerticalMaxUp) / sillyGoose.scaleDisplay);
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
 sillyGoose. readButtonStates();
  if (sillyGoose.upButtonState == LOW)
  {
    letGoOfHardDrop = true;
  }

  if (sillyGoose.downButtonState == HIGH && gameAlive && tetriminoAlive)
  {
    //TODO - implement legit faster fall rules
    tryToMoveDown(tetrimino, true);
  }
  if (sillyGoose.upButtonState == HIGH && letGoOfHardDrop && gameAlive && tetriminoAlive)
  {
    hardDrop(tetrimino);
  }

  if (sillyGoose.leftButtonState == HIGH && gameAlive && tetriminoAlive)
  {
    tryToMoveLeft(tetrimino);
  }
  if (sillyGoose.rightButtonState == HIGH && gameAlive && tetriminoAlive)
  {
    tryToMoveRight(tetrimino);
  }

  if (sillyGoose.aButtonState == HIGH && holdAvailable && gameAlive && tetriminoAlive)
  {
    tetriminoInPlay = false;
  }
  if (sillyGoose.bButtonState == HIGH && letGoOfRotate && gameAlive && tetriminoAlive)
  {

    letGoOfRotate = false;
    tryToRotate(tetrimino);
  }
  if (sillyGoose.bButtonState == LOW)
  {
    letGoOfRotate = true;
  }
  delay(50);
}

void hardDrop(Tetrimino tetrimino)
{
  int hypotheticalVerticalDotPosition = verticalDotPosition + sillyGoose.scaleDisplay;
  int counter = 0;
  while (!hitBottom(tetrimino, hypotheticalVerticalDotPosition) && !overlapOfPlayGrid(tetrimino, horizontalDotPosition, hypotheticalVerticalDotPosition))
  {
    verticalDotPosition = hypotheticalVerticalDotPosition;
    hypotheticalVerticalDotPosition = verticalDotPosition + sillyGoose.scaleDisplay;
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
  int hypotheticalVerticalGhostPosition = verticalGhostPosition + sillyGoose.scaleDisplay;

  while (!hitBottom(tetrimino, hypotheticalVerticalGhostPosition) && !(overlapOfPlayGrid(tetrimino, horizontalDotPosition, hypotheticalVerticalGhostPosition)))
  {
    verticalGhostPosition = hypotheticalVerticalGhostPosition;
    hypotheticalVerticalGhostPosition = verticalGhostPosition + sillyGoose.scaleDisplay;
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
  int hypotheticalVerticalGhostPosition = verticalGhostPosition + sillyGoose.scaleDisplay;

  while (!hitBottom(tetrimino, hypotheticalVerticalGhostPosition) && !(overlapOfPlayGrid(tetrimino, horizontalDotPosition, hypotheticalVerticalGhostPosition)))
  {
    verticalGhostPosition = hypotheticalVerticalGhostPosition;
    hypotheticalVerticalGhostPosition = verticalGhostPosition + sillyGoose.scaleDisplay;
  }
  ghostColour(tetrimino);
}

void tryToMoveDown(Tetrimino tetrimino, bool incrementScore)
{
  //if the block settles above the visable line, end the game.

  int hypotheticalVerticalDotPosition = verticalDotPosition + sillyGoose.scaleDisplay;
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
  if (horizontalDotPosition + (tetrimino.blocksRight() * sillyGoose.scaleDisplay) >= linePosHorizontalMaxRight || horizontalDotPosition + (tetrimino.blocksLeft() * sillyGoose.scaleDisplay) <= linePosHorizontalMaxLeft)

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
  int hypotheticalHorizontalDotPosition = horizontalDotPosition + sillyGoose.scaleDisplay;

  if (hypotheticalHorizontalDotPosition + (tetrimino.blocksRight() * sillyGoose.scaleDisplay) > linePosHorizontalMaxRight)

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
  int hypotheticalHorizontalDotPosition = horizontalDotPosition - sillyGoose.scaleDisplay;

  if (hypotheticalHorizontalDotPosition + (tetrimino.blocksLeft() * sillyGoose.scaleDisplay) < linePosHorizontalMaxLeft)

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
  if (hypotheticalVerticalDotPosition + (tetrimino.blocksDown() * sillyGoose.scaleDisplay) > linePosVerticalMaxDown)
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
        if (playGrid[(((hypotheticalHorizontalDotPosition - linePosHorizontalMaxLeft) / sillyGoose.scaleDisplay) + m) + (tetrisGridCols * (numInvisableRows + ((hypotheticalVerticalDotPosition - linePosVerticalMaxUp) / sillyGoose.scaleDisplay) + n))] != 0)
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

    startHorizontalDotPosition = startHorizontalDotPosition + (0.5 * (4 - tetrimino.cols) * sillyGoose.scaleDisplay);
    if (tetrimino.rows != 4)
    {
      startVerticalDotPosition = startVerticalDotPosition + sillyGoose.scaleDisplay;
      startVerticalDotPosition = startVerticalDotPosition - (0.5 * sillyGoose.scaleDisplay);
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
        if (startVerticalDotPosition + (sillyGoose.scaleDisplay * n) < linePosVerticalMaxUp)
        {
        }
        else
        {
          sillyGoose.TFTscreen.fillRect(startHorizontalDotPosition + 1 + (sillyGoose.scaleDisplay * m), startVerticalDotPosition + 1 + (sillyGoose.scaleDisplay * n), sillyGoose.scaleDisplay - 1, sillyGoose.scaleDisplay - 1, colour);
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
        sillyGoose.TFTscreen.stroke(strokeColour);
        if (startVerticalDotPosition + (sillyGoose.scaleDisplay * n) < linePosVerticalMaxUp)
        {
        }
        else
        {
         sillyGoose.TFTscreen.rect(startHorizontalDotPosition + 1 + (sillyGoose.scaleDisplay * m), startVerticalDotPosition + 1 + (sillyGoose.scaleDisplay * n), sillyGoose.scaleDisplay - 1, sillyGoose.scaleDisplay - 1);
        }
      }
    }
  }
}
