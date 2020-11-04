#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include "yahtzeePlayer.h"
#include "die.h"



void enterMenu();
void enterGame();
void enterUpperSection();
int calculateToak(vector<int> roll);
int calculateFoak(vector<int> roll);
int calculateFullHouse(vector<int> roll);
int calculateSmallStraight(vector<int> roll);
int calculateLargeStraight(vector<int> roll);
int calculateYahtzee(vector<int> roll);
int calculateChance(vector<int> roll);

int calculateScore(int i,vector<int> roll);
int calculateUpper(vector<int> roll, int upper);
int calculateUpperTotal();
int calculateLowerTotal();
void enterScoreSection(vector<int> roll);
string scoreToString(int score);
void updateTuple(vector<int> roll);

int dealWithEndOfRoll();

