#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "dieFacet.h"
using namespace std;

class Die {
    public:
    vector<DieFacet> dieFacets;
    void initialise();
    DieFacet getRandomFace();
    DieFacet getSpecificFace(int faceValue);
    vector<DieFacet> roll (SillyGoose sillyGoose);
    vector<int> rollFacetValues(vector<DieFacet> roll);
    void displayRoll(vector<DieFacet> facets, SillyGoose sillyGoose, vector<bool> held);

   vector<DieFacet> rollHold(SillyGoose sillyGoose, vector<DieFacet> facets, vector<bool> held);



};
