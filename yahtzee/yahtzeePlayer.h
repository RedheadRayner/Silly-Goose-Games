#include <vector>
using namespace std;

class YahtzeePlayer {

  public:
    void setName(char* name);
    char* name;
    int aces;
    int twos;
    int threes;
    int fours;
    int fives;
    int sixes;
    int threeOfAKind;
    int fourOfAKind;
    int fullHouse;
    int smallStraight;
    int largeStraight;
    int yahtzee;
    int chance;
    int yahtzeeBonusChips;

    void calculateAces(vector<int> die);
    void calculateTwos(vector<int> die);
    void calculateThrees(vector<int> die);
    void calculateFours(vector<int> die);
    void calculateFives(vector<int> die);
    void calculateSixes(vector<int> die);
    void calculateThreeOfAKind(vector<int> die);
    void calculateFourOfAKind(vector<int> die);
    void calculateFullHouse(vector<int> die);
    void calculateSmallStraight(vector<int> die);
    void calculateLargeStraight(vector<int> die);
    void calculateYahtzee(vector<int> die);
    void calculateChance(vector<int> die);
    
};