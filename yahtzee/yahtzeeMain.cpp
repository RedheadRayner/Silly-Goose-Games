
#include "yahtzeeMain.h"
#include <set>
#include <numeric>
#include <sys/time.h>

char *scoreFileName = "yahtzeeScores.txt";

int numberOfRounds = 13;

int dieDimension = 21;

int dieGridDimension = 7;

bool dedicatedScore;

SillyGoose sillyGoose;

Die die;

  vector<bool> holdDie;

tuple <string, bool, string> ones = make_tuple("ONES", false, "00");
tuple <string, bool, string> twos = make_tuple("TWOS", false, "00");
tuple <string, bool, string> threes = make_tuple("THREES", false, "00");
tuple <string, bool, string> fours = make_tuple("FOURS", false, "00");
tuple <string, bool, string> fives = make_tuple("FIVES", false, "00");
tuple <string, bool, string> sixes = make_tuple("SIXES", false, "00");
tuple <string, bool, string> totalUpper = make_tuple("TOTAL U", true, "00");


tuple <string, bool, string> toak = make_tuple("THREE OAK", false, "00");
tuple <string, bool, string> foak = make_tuple("FOUR OAK", false, "00");
tuple <string, bool, string> fullHouse = make_tuple("F. HOUSE", false, "00");
tuple <string, bool, string> smallStraight = make_tuple("S. STRT", false, "00");
tuple <string, bool, string> largeStraight = make_tuple("L. STRT", false, "00");
tuple <string, bool, string> yahtzeeScore = make_tuple("YAHTZEE", false, "00");
tuple <string, bool, string> chance = make_tuple("CHANCE", false, "00");
tuple <string, bool, string> totalLower = make_tuple("TOTAL L", true, "00");

vector< tuple <string, bool, string> > scoreBoardTuple = {ones, twos, threes, fours, fives, sixes, totalUpper, toak, foak, fullHouse, smallStraight, largeStraight, yahtzeeScore, chance, totalLower};


int (*scoreCalcFunctPtr[])(vector<int> roll) = { 
 calculateToak,
 calculateFoak,
 calculateFullHouse,
 calculateSmallStraight,
 calculateLargeStraight,
 calculateYahtzee,
 calculateChance } ;

 void (*functptr[])(vector<int> roll) = { 
enterScoreSection } ;

void setup()
{
    srand(analogRead(TFT_DC));
    sillyGoose.initialise(20);
    die.initialise();
    sillyGoose.TFTscreen.background(Black);
    SerialUSB.begin();
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
    int numOfPlayers = playerSetup(sillyGoose) +1;
    // Enter names here...
    std::vector<YahtzeePlayer> players;

    // for (int i = 0; i < numOfPlayers; i++)
    // {
    //     sillyGoose.TFTscreen.background(Black);
    //     sillyGoose.structuredSelectableText("PLAYER", sillyGoose.verticalDotCentrePosition - (2 * sillyGoose.textHeight), false, true, false,false,false);
    //     char str[2];
    //     itoa(i + 1, str, 10);
    //     sillyGoose.structuredSelectableText(str, sillyGoose.verticalDotCentrePosition, false, true, false,false,false);
    //     YahtzeePlayer player;

    //     delay(1000);

    //     player.setName(newPlayer(sillyGoose));
    //     players.push_back(player);
    // }
    for (int k = 0; k <= numberOfRounds; k++)
    {

        
        for (int i = 0; i < numOfPlayers; i++)
        {

            if ( numOfPlayers > 1 ){
            sillyGoose.TFTscreen.background(Black);
            sillyGoose.TFTscreen.textSize(sillyGoose.scaleText * 2);


// sillyGoose.TFTscreen.stroke(White);
//             sillyGoose.TFTscreen.text("IT'S", 5, sillyGoose.verticalDotCentrePosition - (5 * sillyGoose.textHeight));

//             sillyGoose.TFTscreen.text("YOUR", 5, sillyGoose.verticalDotCentrePosition - (3 * sillyGoose.textHeight));

//             sillyGoose.TFTscreen.text("TURN", 5, sillyGoose.verticalDotCentrePosition - sillyGoose.textHeight);
//             sillyGoose.TFTscreen.text(players[i].name, 5, sillyGoose.verticalDotCentrePosition + sillyGoose.textHeight);

//             sillyGoose.TFTscreen.textSize(sillyGoose.scaleText);
            // delay(1000);
            }

                sillyGoose.TFTscreen.background(Black);
        sillyGoose.structuredSelectableText("ROLL", sillyGoose.verticalDotCentrePosition + 21, true, true, false,false,false);
        sillyGoose.waitForA();
        dedicatedScore = false;
        sillyGoose.structuredSelectableText("ROLL", sillyGoose.verticalDotCentrePosition + 21, false, true, false,false,true);
        vector<DieFacet> roll = die.roll(sillyGoose);
        vector<int> rollValues = die.rollFacetValues(roll);

        sillyGoose.structuredSelectableText("ROLL", sillyGoose.verticalDotCentrePosition + 21, true, true, false,false,false);
        sort(rollValues.begin(), rollValues.end());
        holdDie = {false,false,false,false,false};
int r = 1;
        while(!dedicatedScore){
            die.displayRoll(roll, sillyGoose, {false,false,false,false,false});
            sillyGoose.HoldDisplayRectangle( holdDie,sillyGoose.horizontalDotCentrePosition - (5 * (10)) - 6,sillyGoose.verticalDotCentrePosition - (10), 21, 21, 2);

         if(r==3){
                     sillyGoose.structuredSelectableText("ROLL", sillyGoose.verticalDotCentrePosition + 21, false, true, false,false,true);
         sillyGoose.structuredSelectableText(">", sillyGoose.verticalDotCentrePosition + 21, true, true, true,false,false);
             sillyGoose.waitForA();
             enterScoreSection(rollValues);
         }
         else{
        sillyGoose.structuredSelectableText("ROLL", sillyGoose.verticalDotCentrePosition + 21, true, true, false,false,false);
         sillyGoose.structuredSelectableText(">", sillyGoose.verticalDotCentrePosition + 21, false, true, true,false,false);

        int selection = dealWithEndOfRoll();
         if(selection == 0){
             enterScoreSection(rollValues);
         }
         else 
         {
                sillyGoose.structuredSelectableText("ROLL", sillyGoose.verticalDotCentrePosition + 21, false, true, false,false,true);
             r ++;
        roll = die.rollHold(sillyGoose, roll, holdDie);
        rollValues = die.rollFacetValues(roll);
        sort(rollValues.begin(), rollValues.end());
        
   
         }
         }

        
 }
 }
        }
        // Roll screen with 5 die and a roll button
        // click right to go to upper screen and again to lower screen
    }


int dealWithEndOfRoll(){

int selection=1;
while(true){
    sillyGoose. readButtonStates();
    if (sillyGoose.rightButtonState  == HIGH)
    {
        selection=0;
        sillyGoose.structuredSelectableText(">", sillyGoose.verticalDotCentrePosition + 21, true, true, true,false,false);
        sillyGoose.structuredSelectableText("ROLL", sillyGoose.verticalDotCentrePosition + 21, false, true, false,false,false);
      
    }
    if (sillyGoose.leftButtonState  == HIGH)
    {
        selection=1;
        sillyGoose.structuredSelectableText(">", sillyGoose.verticalDotCentrePosition + 21, false, true, true,false,false);
        sillyGoose.structuredSelectableText("ROLL", sillyGoose.verticalDotCentrePosition + 21, true, true, false,false,false);
     
    }
        if (sillyGoose.aButtonState  == HIGH)
    {
        return selection;
    }

    if(sillyGoose.upButtonState == HIGH){
                sillyGoose.structuredSelectableText(">", sillyGoose.verticalDotCentrePosition + 21, false, true, true,false,false);
        sillyGoose.structuredSelectableText("ROLL", sillyGoose.verticalDotCentrePosition + 21, false, true, false,false,false);
        
        holdDie = sillyGoose.menuOptionsRectangle(  holdDie, sillyGoose.horizontalDotCentrePosition - (5 * (10)) - 6,  sillyGoose.verticalDotCentrePosition - (10), 21, 21, 2);
        sillyGoose.structuredSelectableText("ROLL", sillyGoose.verticalDotCentrePosition + 21, true, true, false,false,false);
        

    }
}
return selection;

}


void enterScoreSection(vector<int> roll)
{
    // print the options
    sillyGoose.TFTscreen.background(Black);
    updateTuple( roll);
    // need to be able to go left if you can roll again...
    int selected = sillyGoose.menuOptionsTuple(3,scoreBoardTuple , 2);
    if(selected == -1){
        sillyGoose.TFTscreen.background(Black);
        return;
    }
    dedicatedScore = true;
    get<1>(scoreBoardTuple[selected]) = true;
    sillyGoose.TFTscreen.background(Black);

}

void updateTuple(vector<int> roll){

sillyGoose.TFTscreen.text( scoreToString(roll[0]).c_str(), 65, 5);
sillyGoose.TFTscreen.text( scoreToString(roll[1]).c_str(), 75, 5);
sillyGoose.TFTscreen.text( scoreToString(roll[2]).c_str(), 85, 5);
sillyGoose.TFTscreen.text( scoreToString(roll[3]).c_str(), 95, 5);
sillyGoose.TFTscreen.text( scoreToString(roll[4]).c_str(), 105, 5);

 for (int i = 0; i <= 5; i++){
  
  if(  !get<1>(scoreBoardTuple[i]) ){
 get<2>(scoreBoardTuple[i]) =scoreToString(calculateUpper( roll, i+1));
  }
 }

 get<2>(scoreBoardTuple[6]) = scoreToString(calculateUpperTotal());
   for (int i = 7; i < scoreBoardTuple.size(); i++)
  {
  if(  !get<1>(scoreBoardTuple[i]) ){
 get<2>(scoreBoardTuple[i]) = scoreToString(calculateScore( i-7, roll));
  }
  }

  
 get<2>(scoreBoardTuple[14]) = scoreToString(calculateLowerTotal( ));

}

int calculateScore(int i, vector<int> roll){
    return (*scoreCalcFunctPtr[i])(roll);

}

int calculateUpper(vector<int> roll, int upper){
    int score = 0;
    for (int value : roll){
        if (value == upper ){
            score += upper;
        }
    }
    return score;


}int calculateUpperTotal( ){

    int score = 0;
    for (int i = 0; i < 5; i++){
        if( get<1>(scoreBoardTuple[i]) ){
            string::size_type sz;
 score += stoi (get<2>(scoreBoardTuple[i]),&sz);  ;}
    }
if (score > 63){
     get<0>(scoreBoardTuple[6]) = "TOTAL U+B";
    return score + 35;
}
return score;
}int calculateLowerTotal(){
    int score = 0;
    for (int i = 7; i < 13; i++){
        if( get<1>(scoreBoardTuple[i]) ){
            string::size_type sz;
 score += stoi (get<2>(scoreBoardTuple[i]),&sz);  ;}
    }

return score;
}


int calculateToak(vector<int> roll){
  int counter = 0;

    int prev = roll[0];
    for (int i = 1 ; i < roll.size() ; i ++ ){
    if(roll[i] == prev){
        counter ++;
    if (counter >= 2){
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
int calculateFullHouse(vector<int> roll){
        if(set<int>( roll.begin(), roll.end() ).size()!=2){
           
        return 0;
    }
    if ( roll[0] != roll[1] || roll[3] != roll[4]){
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
    if (counter >= 3){
        return 30;
    }
        
    }else if(roll[i] == prev){
            // you should allow, for instance, 12234 where you have 1234 as the small straight and 2 as the odd die.
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

string scoreToString(int score){
    int digNum = getNumberOfDigits(score); 
 char pos[digNum];
    sprintf(pos, "%i", score);

    char *returnable;
if ( digNum == 1){
    returnable = (char *)malloc(digNum + 2 );
    returnable[0] = ' ';
     returnable[1] = pos[0];
} else{
returnable = (char *)malloc(digNum +1 );
    for (int i = 0; i <digNum; i++)
    {
        returnable[i] = pos[i];
    }
}    

returnable[2] = '\0';
    
    return returnable;
}

