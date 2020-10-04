
#include "libraries/gameplayFunctions.h"
#include <vector>
extern "C"
{
#include <TFT.h> 
#include "Arduino.h"
#include "utility/Adafruit_GFX.h"
}
using namespace std;

class DieFacet
{
public:
    void initialise(vector<std::pair<double, double>> dieDots, int numberRep);
    void drawFacet(SillyGoose sillyGoose, int dieCornerX, int dieCornerY, int scale);
    vector<std::pair<double, double>> dieDots;
    int numberRep;
};
