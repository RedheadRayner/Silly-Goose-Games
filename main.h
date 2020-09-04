
#ifndef MAIN_H
#define MAIN_H

#include "tetrimino.h"
#include <Arduino.h>

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
void waitForB();
void structuredSelectableText(char *text, int height, bool selected, bool title, bool rightHandPushed);
void readButtonStates();
void errorScreen(char *message);
void scoreBoard(bool trueScore);
char* position(int place);

unsigned getNumberOfDigits(unsigned i);

# endif