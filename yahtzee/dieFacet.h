
#include "libraries/GameplayFunctions.h"
#include <vector>

class DieFacet
{
public:
    void initialise(vector<std::pair<double, double>> dieDots, int numberRep);
    void drawFacet(SillyGoose sillyGoose, int dieCornerX, int dieCornerY, int scale);
    vector<std::pair<double, double>> dieDots;
    int numberRep;
};
