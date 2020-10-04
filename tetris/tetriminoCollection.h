
#include <Arduino.h>
#include <stdint.h>
#include "tetrimino.h"
#include "libraries/gameplayFunctions.h"
#include <algorithm>
#include <vector>

using namespace std;


class TetriminoCollection {

  public:
    // the 7 tetriminos in an array to be chosen from
    std::vector<Tetrimino> tetriminoArray;
    std::vector<Tetrimino> getRandomSet();

    void initialise(int scale);
};
