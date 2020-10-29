#include "die.h"
#include <vector>

using namespace std;

int rolls = 10;

void Die::initialise()
{
    DieFacet one;
    DieFacet two;
    DieFacet three;
    DieFacet four;
    DieFacet five;
    DieFacet six;

    double dotOne = 1.5;
    double dotTwo = 3.5;
    double dotThree = 5.5;

    one.initialise({{dotTwo, dotTwo}}, 1);
    two.initialise({{dotOne, dotOne}, {dotThree, dotThree}}, 2);
    three.initialise({{dotOne, dotOne}, {dotTwo, dotTwo}, {dotThree, dotThree}}, 3);
    four.initialise({{dotOne, dotOne}, {dotOne, dotThree}, {dotThree, dotOne}, {dotThree, dotThree}}, 4);
    five.initialise({{dotOne, dotOne}, {dotOne, dotThree}, {dotTwo, dotTwo}, {dotThree, dotOne}, {dotThree, dotThree}}, 5);
    six.initialise({{dotOne, dotOne}, {dotOne, dotTwo}, {dotOne, dotThree}, {dotThree, dotOne}, {dotThree, dotTwo}, {dotThree, dotThree}}, 6);
    this->dieFacets.push_back(one);
    this->dieFacets.push_back(two);
    this->dieFacets.push_back(three);
    this->dieFacets.push_back(four);
    this->dieFacets.push_back(five);
    this->dieFacets.push_back(six);
}

DieFacet Die::getRandomFace()
{
    // TODO use uptime random seed
    int random = (rand() % 6);

    return this->dieFacets[random];
}

DieFacet Die::getSpecificFace(int faceValue)
{

    return this->dieFacets[faceValue - 1];
}

vector<int> Die::roll(SillyGoose sillyGoose)
{
    vector<int> facetsValue;
    vector<DieFacet> facets;

    for (int i = 0; i <= rolls; i++)
    {

        for (int i = 0; i < 6; i++)
        {
            facets.push_back(getRandomFace());
        }
        displayRoll(facets, sillyGoose);
        if (i != rolls - 1)
        {
            facets.clear();
        }
        delay(100);
    }

    for(DieFacet facet : facets){
 facetsValue.push_back(facet.numberRep);
    }
    return facetsValue;
}

void Die::displayRoll(vector<DieFacet> facets, SillyGoose sillyGoose)
{
    
    facets[0].drawFacet(sillyGoose, sillyGoose.horizontalDotCentrePosition - (3 * (10)) - 3, sillyGoose.verticalDotCentrePosition - (10), 3);
    
    facets[1].drawFacet(sillyGoose, sillyGoose.horizontalDotCentrePosition - (5 * (10)) - 6, sillyGoose.verticalDotCentrePosition - (10), 3);
    
    facets[2].drawFacet(sillyGoose, sillyGoose.horizontalDotCentrePosition - (10), sillyGoose.verticalDotCentrePosition - (10), 3);

    facets[3].drawFacet(sillyGoose, sillyGoose.horizontalDotCentrePosition + ((10)) + 3, sillyGoose.verticalDotCentrePosition - (10), 3);
    
    facets[4].drawFacet(sillyGoose, sillyGoose.horizontalDotCentrePosition + (3*10) +6, sillyGoose.verticalDotCentrePosition - (10), 3);

}