#include "dieFacet.h"

void DieFacet::initialise(vector<std::pair<double, double>> dieDots, int numberRep)
{
    this->dieDots = dieDots;
    this->numberRep = numberRep;
}

void DieFacet::drawFacet(SillyGoose sillyGoose, int dieCornerX, int dieCornerY, int scale)
{
sillyGoose.TFTscreen.fillRoundRect(dieCornerX, dieCornerY, scale * 7, scale * 7,1,  White);

    vector<std::pair<double, double>> dieDots = this->dieDots;

    for (std::pair<double, double> dieDot : dieDots)
    {
        
        sillyGoose.TFTscreen.fillCircle(dieCornerX + (scale * dieDot.first), dieCornerY + (scale * dieDot.second),2, Black);
    }
}
