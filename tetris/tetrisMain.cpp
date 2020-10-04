#ifdef TETRIS
#include "tetrisMain.h"

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

char *scoreFileName = "scores.txt";

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

  checkForScores(sillyGoose, scoreFileName);
}

void loop()
{
  enterMenu();
}

// Main menu selector
void enterMenu()
{
  int selected = menuSetup(sillyGoose, "JETRIS");
  switch (selected)
  {
  case 0:
    enterGame();
    break;
  case 1:
    enterScores(sillyGoose, scoreFileName);
    break;
  case 2:
    enterSettings(sillyGoose);
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
    randomTetriminos.push_back(randomTetriminosSet[index]);
  }

  randomTetriminosSet = tetriminoCollection.getRandomSet();
  for (int index = 0; index < 7; index++)
  {
    randomTetriminos.push_back(randomTetriminosSet[index]);
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
  enterGameEndedPhase(sillyGoose, scoreFileName, score);
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
  sillyGoose.readButtonStates();
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

#endif