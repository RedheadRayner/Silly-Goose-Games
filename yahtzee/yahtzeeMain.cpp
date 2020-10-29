
#include "yahtzeeMain.h"
#include <time.h>
#include <set>
#include <numeric>

char *scoreFileName = "yahtzeeScores.txt";

int numberOfRounds = 13;

int dieDimension = 21;

int dieGridDimension = 7;

SillyGoose sillyGoose;

Die die;

tuple <string, bool, string> ones = make_tuple("ONES", false, "0");
tuple <string, bool, string> twos = make_tuple("TWOS", false, "0");
tuple <string, bool, string> threes = make_tuple("THREES", false, "0");
tuple <string, bool, string> fours = make_tuple("FOURS", false, "0");
tuple <string, bool, string> fives = make_tuple("FIVES", false, "0");
tuple <string, bool, string> sixes = make_tuple("SIXES", false, "0");
tuple <string, bool, string> totalUpper = make_tuple("TOTAL", true, "0");


tuple <string, bool, string> toak = make_tuple("THREE OF A KIND", false, "0");
tuple <string, bool, string> foak = make_tuple("FOUR OF A KIND", false, "0");
tuple <string, bool, string> fullHouse = make_tuple("FULL HOUSE", false, "0");
tuple <string, bool, string> smallStraight = make_tuple("SMALL STRAIGHT", false, "0");
tuple <string, bool, string> largeStraight = make_tuple("LARGE STRAIGHT", false, "0");
tuple <string, bool, string> yahtzeeScore = make_tuple("YAHTZEE", false, "0");
tuple <string, bool, string> chance = make_tuple("CHANCE", false, "0");
tuple <string, bool, string> totalLower = make_tuple("TOTAL", true, "0");

vector< tuple <string, bool, string> > scoreBoardTuple = {ones, twos, threes, fours, fives, sixes, totalUpper, toak, foak, fullHouse, smallStraight, largeStraight, yahtzeeScore, chance, totalLower};


int (*functptr[])(vector<int> roll) = { 
 calculateToak,
 calculateFoak,
 calculateFullHouse,
 calculateSmallStraight,
 calculateLargeStraight,
 calculateYahtzee,
 calculateChance } ;

void setup()
{
    sillyGoose.initialise(20);
    die.initialise();
    SerialUSB.begin();
    srand(time(NULL));
}

void loop()
{
    enterMenu();
}

// Main menu selector
void enterMenu()
{
    int selected = sillyGoose.startMenu( "JAHTZEE");
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
        sillyGoose.structuredSelectableText("PLAYER", sillyGoose.verticalDotCentrePosition - (2 * sillyGoose.textHeight), false, true, false,false,false);
        char str[2];
        itoa(i + 1, str, 10);
        sillyGoose.structuredSelectableText(str, sillyGoose.verticalDotCentrePosition, false, true, false,false,false);
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
        sillyGoose.structuredSelectableText("ROLL", sillyGoose.verticalDotCentrePosition + 21, true, true, false,false,false);
        sillyGoose.waitForA();
        vector<int> roll = die.roll(sillyGoose);
        sort(roll.begin(), roll.end());
        sillyGoose.structuredSelectableText("ROLL", sillyGoose.verticalDotCentrePosition + 21, false, true, false,false,false);
        // sillyGoose.TFTscreen.fillTriangle(sillyGoose.screenLong - 20, sillyGoose.verticalDotCentrePosition - 20, sillyGoose.screenLong - 10, sillyGoose.verticalDotCentrePosition, sillyGoose.screenLong - 20, sillyGoose.verticalDotCentrePosition + 20,White);
        sillyGoose.waitForRight();

        enterScoreSection(roll);

        delay(5000);

        // Roll screen with 5 die and a roll button
        // click right to go to upper screen and again to lower screen
    }
}

void enterScoreSection(vector<int> roll)
{
    // print the options
            sillyGoose.TFTscreen.background(Black);

    int selected = sillyGoose.menuOptionsTuple(1,scoreBoardTuple , 1);

    sillyGoose.TFTscreen.background(Black);

    while (true)
    {
    }
}

void updateTuple(vector<tuple <string, bool, string>> items, vector<int> roll){
 for (int i = 0; i < 5; i++){
  
  if(  !get<1>(items[i]) ){
 get<2>(items[i]) = scoreToString(calculateUpper(items, roll, i));
  }
 }

 get<2>(items[6]) = scoreToString(calculateUpperTotal(items ));
   for (int i = 0; i < items.size(); i++)
  {
  if(  !get<1>(items[i]) ){
 get<2>(items[i]) = scoreToString(calculateScore( i-6, roll));
  }
  }

  
 get<2>(items[14]) = scoreToString(calculateLowerTotal( items));

}

int calculateScore(int i, vector<int> roll){
    return (*functptr[i])(roll);

}

int calculateUpper(vector<tuple <string, bool, string>> items, vector<int> roll, int upper){
    int score = 0;
    for (int value : roll){
        if (value == upper ){
            score += upper;
        }
    }
    return score;


}int calculateUpperTotal(vector<tuple <string, bool, string>> items ){

    int score = 0;
    for (int i = 0; i < 5; i++){
        if( get<1>(items[i]) ){
            string::size_type sz;
 score += stoi (get<2>(items[i]),&sz);  ;}
    }
if (score > 63){
     get<0>(items[6]) = "TOTAL + BONUS";
    return score + 35;
}
return score;
}int calculateLowerTotal(vector<tuple <string, bool, string>> items){
    int score = 0;
    for (int i = 7; i < 13; i++){
        if( get<1>(items[i]) ){
            string::size_type sz;
 score += stoi (get<2>(items[i]),&sz);  ;}
    }

return score;
}


int calculateToak(vector<int> roll){
  int counter = 0;
    int prev = roll[0];
    for (int i = 1 ; i < roll.size() ; i ++ ){
    if(roll[i] == prev){
        counter ++;
    if (counter >= 3){
        return accumulate(roll.begin(),roll.end(),0);
    }
        
    }else{
        counter =0;
    }
    prev = roll[i];


    }
    return 0;
}
int calculateFoak(vector<int> roll){

  int counter = 0;
    int prev = roll[0];
    for (int i = 1 ; i < roll.size() ; i ++ ){
    if(roll[i] == prev){
        counter ++;
    if (counter >= 4){
        return accumulate(roll.begin(),roll.end(),0);
    }
        
    }else{
        counter =0;
    }
    prev = roll[i];


    }
    return 0;
}
int calculateFullHouse(vector<int> roll){
        if(set<int>( roll.begin(), roll.end() ).size()!=2){
        return 0;
    }
    if ( roll[0] != roll[1] || roll[4]!= roll[5]){
        return 0;
    }
    return 25;
}
int calculateSmallStraight(vector<int> roll){
    int counter = 0;
    int prev = roll[0];
    for (int i = 1 ; i < roll.size() ; i ++ ){
    if(roll[i] == prev +1){
        counter ++;
    if (counter >= 4){
        return 30;
    }
        
    }else{
        counter =0;
    }
    prev = roll[i];


    }
    return 0;
}
int calculateLargeStraight(vector<int> roll){
    int prev = roll[0];
    for (int i = 1 ; i < roll.size() ; i ++ ){
    if(roll[i] != prev +1){

        return 0;
    }
    prev = roll[i];
    }
     return 50;
}
int calculateYahtzee(vector<int> roll){
    int prev = roll[0];
    for (int i = 1 ; i < roll.size() ; i ++ ){
    if(roll[i] != prev){

        return 0;
    }
    prev = roll[i];
    }
    return 50;
}
int calculateChance(vector<int> roll){
    int score=0;
    for (int die : roll){
        score += die;
    }
    return score;
}



