#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include "tetriminoCollection.h"

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
void enterMenu();
void enterGame();
void updateScore();
void updateLevel();
