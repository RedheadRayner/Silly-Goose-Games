
#include "yahtzeeMain.h"

using namespace std;

char *scoreFileName = "yahtzeeScores.txt";

int numberOfRounds = 13;

int dieDimension = 21;

int dieGridDimension = 7;

SillyGoose sillyGoose;

Die die;

void setup()
{
    sillyGoose.initialise(20);
    die.initialise();
    SerialUSB.begin();
}

void loop()
{
    enterMenu();
}

// Main menu selector
void enterMenu()
{
    int selected = menuSetup(sillyGoose, "JAHTZEE");
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

void enterGame()
{
    // Number of players selector (1 or 2):
    int selected = playerSetup(sillyGoose, "PLAYERS");
    // Enter names here...
    std::vector<YahtzeePlayer> players;

    for (int i = 0; i < selected; i++)
    {
        sillyGoose.TFTscreen.background(Black);
        sillyGoose.structuredSelectableText("PLAYER", sillyGoose.verticalDotCentrePosition - (2 * sillyGoose.textHeight), false, true, false);
        char str[2];
        itoa(i + 1, str, 10);
        sillyGoose.structuredSelectableText(str, sillyGoose.verticalDotCentrePosition, false, true, false);
        YahtzeePlayer player;

        delay(1000);

        player.setName(newPlayer(sillyGoose));
        players.push_back(player);
    }
    for (int k = 0; k <= numberOfRounds; k++)
    {
        for (int i = 0; i < selected; i++)
        {
            sillyGoose.TFTscreen.background(Black);
            sillyGoose.TFTscreen.textSize(sillyGoose.scaleText * 2);

            sillyGoose.TFTscreen.text("IT'S", 5, sillyGoose.verticalDotCentrePosition - (5 * sillyGoose.textHeight));

            sillyGoose.TFTscreen.text("YOUR", 5, sillyGoose.verticalDotCentrePosition - (3 * sillyGoose.textHeight));

            sillyGoose.TFTscreen.text("TURN", 5, sillyGoose.verticalDotCentrePosition - sillyGoose.textHeight);
            sillyGoose.TFTscreen.text(players[i].name, 5, sillyGoose.verticalDotCentrePosition + sillyGoose.textHeight);

            sillyGoose.TFTscreen.textSize(sillyGoose.scaleText);
            delay(1000);
        }

        sillyGoose.TFTscreen.background(Black);
        sillyGoose.structuredSelectableText("ROLL", sillyGoose.verticalDotCentrePosition + 21, true, true, false);
        sillyGoose.waitForA();
        vector<DieFacet> roll = die.roll(sillyGoose);
        sillyGoose.structuredSelectableText("ROLL", sillyGoose.verticalDotCentrePosition + 21, false, true, false);
        sillyGoose.TFTscreen.fillTriangle(sillyGoose.screenLong - 20, sillyGoose.verticalDotCentrePosition - 20, sillyGoose.screenLong - 10, sillyGoose.verticalDotCentrePosition, sillyGoose.screenLong - 20, sillyGoose.verticalDotCentrePosition + 20,White);
        sillyGoose.waitForRight();

        enterUpperSection();

        delay(5000);

        // Roll screen with 5 die and a roll button
        // click right to go to upper screen and again to lower screen
    }
}

void enterUpperSection()
{

    sillyGoose.structuredSelectableText("ONES", 10, true, false, false);
    sillyGoose.structuredSelectableText("TWOS", 10 + sillyGoose.scaleText, false, false, false);
    sillyGoose.structuredSelectableText("THREES", 10 + (2 * sillyGoose.scaleText), false, false, false);
    sillyGoose.structuredSelectableText("FOURS", 10 + (3 * sillyGoose.scaleText), false, false, false);
    sillyGoose.structuredSelectableText("FIVES", 10 + (4 * sillyGoose.scaleText), false, false, false);
    sillyGoose.structuredSelectableText("SIXES", 10 + (5 * sillyGoose.scaleText), false, false, false);
    sillyGoose.structuredSelectableText("TOTAL", 10 + (6 * sillyGoose.scaleText), false, false, false);

    sillyGoose.structuredSelectableText("THREE OF A KIND", 10 + (7 * sillyGoose.scaleText), false, false, false);
    sillyGoose.structuredSelectableText("FOUR OF A KIND", 10 + (8 * sillyGoose.scaleText), false, false, false);
    sillyGoose.structuredSelectableText("FULL HOUSE", 10 + (9 * sillyGoose.scaleText), false, false, false);
    sillyGoose.structuredSelectableText("SMALL STRAIGHT", 10 + (10 * sillyGoose.scaleText), false, false, false);
    sillyGoose.structuredSelectableText("LARGE STRAIGHT", 10 + (11 * sillyGoose.scaleText), false, false, false);
    sillyGoose.structuredSelectableText("YAHTZEE", 10 + (12 * sillyGoose.scaleText), false, false, false);
    sillyGoose.structuredSelectableText("CHANCE", 10 + (13 * sillyGoose.scaleText), false, false, false);
    sillyGoose.structuredSelectableText("TOTAL", 10 + (14 * sillyGoose.scaleText), false, false, false);
    while (true)
    {
    }
}
