#include "gameplayFunctions.h"

// Allows the player to create a name to save to the scoreboard.
char *newPlayer(SillyGoose sillyGoose)
{
    sillyGoose.TFTscreen.background(Black);
    sillyGoose.structuredSelectableText("ENTER", sillyGoose.verticalDotCentrePosition - (7 * sillyGoose.textHeight), false, true, false);
    sillyGoose.structuredSelectableText("YOUR", sillyGoose.verticalDotCentrePosition - (5 * sillyGoose.textHeight), false, true, false);
    sillyGoose.structuredSelectableText("NAME", sillyGoose.verticalDotCentrePosition - (3 * sillyGoose.textHeight), false, true, false);
    char *characters[27] = {" ", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"};
    int letters[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int charactSelec = 0;
    makeSelectionScoreArrows(sillyGoose);
    bool letGoOfA = true;
    bool letGoOfB = true;

    textNameUnderscore(sillyGoose, charactSelec, Green);

    while (true)
    {
        sillyGoose.readButtonStates();

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

            changeSelectionScoreName(sillyGoose, charactSelec, characters[letters[charactSelec]], Green);
            changeSelectionScoreArrows(sillyGoose, charactSelec, false, Green);
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

            changeSelectionScoreName(sillyGoose, charactSelec, characters[letters[charactSelec]], Green);
            changeSelectionScoreArrows(sillyGoose, charactSelec, true, Green);
        }
        if (sillyGoose.upButtonState == LOW)
        {
            changeSelectionScoreArrows(sillyGoose, charactSelec, true, White);
        }
        if (sillyGoose.downButtonState == HIGH)
        {
            changeSelectionScoreArrows(sillyGoose, charactSelec, false, White);
        }

        if (sillyGoose.aButtonState == HIGH && letGoOfA)
        {
            letGoOfA = false;
            changeSelectionScoreName(sillyGoose, charactSelec, characters[letters[charactSelec]], White);
            textNameUnderscore(sillyGoose, charactSelec, Black);
            if (charactSelec == 7)
            {
                break;
            }
            else
            {
                charactSelec++;
                textNameUnderscore(sillyGoose, charactSelec, Green);
            }
        }

        if (sillyGoose.bButtonState == HIGH && letGoOfB)
        {
            letGoOfB = false;

            if (charactSelec != 0)
            {
                textNameUnderscore(sillyGoose, charactSelec, Black);
                changeSelectionScoreName(sillyGoose, charactSelec, characters[letters[charactSelec]], White);
                charactSelec--;
                textNameUnderscore(sillyGoose, charactSelec, Green);
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
void enterGameEndedPhase(SillyGoose sillyGoose, char *scoreFileName, int score)
{

    sillyGoose.TFTscreen.background(Black);
    char *name = newPlayer(sillyGoose);
    sillyGoose.TFTscreen.background(Black);
    sillyGoose.structuredSelectableText("PLEASE", sillyGoose.verticalDotCentrePosition - (2 * sillyGoose.textHeight), false, true, false);
    sillyGoose.structuredSelectableText("WAIT", sillyGoose.verticalDotCentrePosition, false, true, false);

    int scoreLinesCounting = 0;
    String restOfFile = "";
    int place = 1;
    File scoreFile = SD.open(scoreFileName);

    while (scoreFile.available() && place < 1001)
    {
        int i = 10;
        if (i = 20)
        {
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
    scoreFile = SD.open(scoreFileName, FILE_WRITE);
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
    scoreBoard(sillyGoose, scoreFileName, true, true);
}

char *position(int place)
{
    char *suffix;
    int lastDigit = place % 10;
    if (lastDigit == 1)
    {
        suffix = "st";
    }
    else if (lastDigit == 2)
    {
        suffix = "nd";
    }
    else if (lastDigit == 3)
    {
        suffix = "rd";
    }
    else
    {
        suffix = "th";
    }
    char pos[getNumberOfDigits(place)];
    sprintf(pos, "%i", place);

    char *returnable;

    returnable = (char *)malloc(getNumberOfDigits(place) + 3);
    for (int i = 0; i < getNumberOfDigits(place); i++)
    {
        returnable[i] = pos[i];
    }
    returnable[getNumberOfDigits(place)] = suffix[0];

    returnable[getNumberOfDigits(place) + 1] = suffix[1];

    returnable[getNumberOfDigits(place) + 2] = '\0';

    return returnable;
}

unsigned getNumberOfDigits(unsigned i)
{
    return i > 0 ? (int)log10((double)i) + 1 : 1;
}

void changeSelectionScoreName(SillyGoose sillyGoose, int letterNum, char *character, uint16_t colour)
{

    int textX = (0.5 * sillyGoose.screenShort) - 24 + (letterNum * sillyGoose.textWidth);

    sillyGoose.TFTscreen.fillRect(textX, sillyGoose.verticalDotCentrePosition + sillyGoose.textHeight, sillyGoose.textWidth, sillyGoose.textHeight, Black);

    sillyGoose.TFTscreen.stroke(colour);

    sillyGoose.TFTscreen.text(character, textX, sillyGoose.verticalDotCentrePosition + sillyGoose.textHeight);
}

void makeSelectionScoreArrows(SillyGoose sillyGoose)
{
    for (int i = 0; i < 8; i++)
    {
        changeSelectionScoreArrows(sillyGoose, i, true, White);
        changeSelectionScoreArrows(sillyGoose, i, false, White);
    }
}

void changeSelectionScoreArrows(SillyGoose sillyGoose, int letterNum, bool up, uint16_t colour)
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

void textNameUnderscore(SillyGoose sillyGoose, int letterNum, uint16_t colour)

{

    int textX = (0.5 * sillyGoose.screenShort) - 24 + (letterNum * sillyGoose.textWidth);

    sillyGoose.TFTscreen.drawRect(textX, sillyGoose.verticalDotCentrePosition + (2 * sillyGoose.textHeight), sillyGoose.textWidth, 2, colour);
}

void checkForScores(SillyGoose sillyGoose, char *scoreFileName)
{
    File scoreFile = SD.open(scoreFileName, FILE_WRITE);
    scoreFile.close();
    if (!SD.exists(scoreFileName))
    {
        sillyGoose.errorScreen("SCORES FILE BROKEN");
    }
}

void scoreBoard(SillyGoose sillyGoose, char *scoreFileName, bool trueScore, bool afterGame)
{
    File scoreFile = SD.open(scoreFileName);
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
    if(!afterGame){
    enterScores(sillyGoose, scoreFileName);
    }
}


void enterScores(SillyGoose sillyGoose,char *scoreFileName)
{

  sillyGoose.TFTscreen.background(Black);
  sillyGoose.TFTscreen.stroke(White);

  int selected = scoreTypeSelection(sillyGoose);

  switch (selected)
  {
  case 0:
    scoreBoard(sillyGoose,scoreFileName, true, false);
    break;
  case 1:
    scoreBoard(sillyGoose,scoreFileName, false, false);
    break;
    case 2:
    // Go back to the menu..
    break;
  default:
    sillyGoose.errorScreen("INVALID INT");
  }
}

int scoreTypeSelection(SillyGoose sillyGoose)
{

  sillyGoose.structuredSelectableText("SCORE MODE", sillyGoose.verticalDotCentrePosition - (4 * sillyGoose.textHeight), false, true, false);

  bool letGoOfUp = true;
  bool letGoOfDown = true;

  int selected = 0;
  changeSelectionScoreMenu(sillyGoose,selected);

  while (true)
  {
    sillyGoose.readButtonStates();

    if (sillyGoose.downButtonState == HIGH && letGoOfDown)
    {
      if (selected < 1)
      {
        selected++;
        changeSelectionScoreMenu(sillyGoose,selected);
      }
      letGoOfDown = false;
    }
    if (sillyGoose.upButtonState == HIGH && letGoOfUp)
    {
      if (selected > 0)
      {
        selected--;
        changeSelectionScoreMenu(sillyGoose,selected);
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
      return 2;
    }

    delay(50);
  }
}

void changeSelectionScoreMenu(SillyGoose sillyGoose,int selection)
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

void enterSettings(SillyGoose sillyGoose)
{

  sillyGoose.TFTscreen.background(Yellow);
  sillyGoose.TFTscreen.stroke(Black);
  sillyGoose.TFTscreen.text("UNDER CONSTRUCTION :(", 0, 0);

  sillyGoose.waitForB();
}

int menuSetup(SillyGoose sillyGoose, char* title)
{
  sillyGoose.TFTscreen.background(Black);
  sillyGoose.structuredSelectableText(title, sillyGoose.verticalDotCentrePosition - (4 * sillyGoose.textHeight), false, true, false);

  bool letGoOfUp = true;
  bool letGoOfDown = true;

  int selected = 0;
  changeSelectionStartMenu(sillyGoose,selected);

  while (true)
  {
    sillyGoose.readButtonStates();

    if (sillyGoose.downButtonState == HIGH && letGoOfDown)
    {
      if (selected < 2)
      {
        selected++;
        changeSelectionStartMenu(sillyGoose,selected);
      }
      letGoOfDown = false;
    }
    if (sillyGoose.upButtonState == HIGH && letGoOfUp)
    {
      if (selected > 0)
      {
        selected--;
        changeSelectionStartMenu(sillyGoose,selected);
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

void changeSelectionStartMenu(SillyGoose sillyGoose,int selection)
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

int playerSetup(SillyGoose sillyGoose, char* title)
{
  sillyGoose.TFTscreen.background(Black);
  sillyGoose.structuredSelectableText(title, sillyGoose.verticalDotCentrePosition - (4 * sillyGoose.textHeight), false, true, false);

  bool letGoOfUp = true;
  bool letGoOfDown = true;

  int selected = 1;
  changeSelectionPlayerNumberMenu(sillyGoose,selected);

  while (true)
  {
    sillyGoose.readButtonStates();

    if (sillyGoose.downButtonState == HIGH && letGoOfDown)
    {
      if (selected < 2)
      {
        selected++;
        changeSelectionPlayerNumberMenu(sillyGoose,selected);
      }
      letGoOfDown = false;
    }
    if (sillyGoose.upButtonState == HIGH && letGoOfUp)
    {
      if (selected > 1)
      {
        selected--;
        changeSelectionPlayerNumberMenu(sillyGoose,selected);
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

void changeSelectionPlayerNumberMenu(SillyGoose sillyGoose,int selection)
{
  switch (selection)
  {
  case 1:

    sillyGoose.structuredSelectableText("ONE", sillyGoose.verticalDotCentrePosition - sillyGoose.textHeight, true, false, false);
    sillyGoose.structuredSelectableText("TWO", sillyGoose.verticalDotCentrePosition + sillyGoose.textHeight, false, false, false);
    break;
  case 2:
    sillyGoose.structuredSelectableText("ONE", sillyGoose.verticalDotCentrePosition - sillyGoose.textHeight, false, false, false);
    sillyGoose.structuredSelectableText("TWO", sillyGoose.verticalDotCentrePosition + sillyGoose.textHeight, true, false, false);

    break;
  default:
    sillyGoose.errorScreen("INVALID INT");
  }

}


