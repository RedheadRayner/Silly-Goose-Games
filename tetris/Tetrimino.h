#include <Arduino.h>
#include <stdint.h>

class Tetrimino {

  public:
    void initialise(int gridPass[], int dim, uint16_t tetColour, int scaleDisplay);
    virtual int blocksRight();
    virtual int blocksLeft();
    virtual int blocksDown();
    virtual bool booleanOfGrid(int,int);
    virtual bool booleanOfStartGrid(int,int);
    void rotateGrid();
    void unrotateGrid();
    void spawn();
    int rows;
    int cols;
    int* startGrid;
    int* currentGrid;
    uint16_t colour;
};
