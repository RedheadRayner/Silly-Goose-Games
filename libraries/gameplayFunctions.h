
#include "SillyGoose.h"


char *newPlayer(SillyGoose sillyGoose);

void enterGameEndedPhase(SillyGoose sillyGoose, char *scoreFileName, int score);
char *position(int place);
unsigned getNumberOfDigits(unsigned i);
void changeSelectionScoreName(SillyGoose sillyGoose, int letterNum, char *character, uint16_t colour);
void makeSelectionScoreArrows(SillyGoose sillyGoose);
void changeSelectionScoreArrows(SillyGoose sillyGoose, int letterNum, bool up, uint16_t colour);
void textNameUnderscore(SillyGoose sillyGoose, int letterNum, uint16_t colour);
void checkForScores(SillyGoose sillyGoose, char *scoreFileName);
void scoreBoard(SillyGoose sillyGoose, char *scoreFileName, bool trueScore, bool afterGame);
void enterScores(SillyGoose sillyGoose, char *scoreFileName);
int scoreTypeSelection(SillyGoose sillyGoose);
void enterSettings(SillyGoose sillyGoose);
int playerSetup(SillyGoose sillyGoose, char* title);