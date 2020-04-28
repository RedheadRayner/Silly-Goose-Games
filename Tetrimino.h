#ifndef TET_H
#define TET_H

class Tetrimino {

  public:
    void initialise(boolean *gridPass, int rowNum, int colNum, uint16_t tetColour, int multiplier);
    virtual int blocksRight();
    virtual int blocksDown();
    virtual boolean booleanOfGrid(int,int);
    int rows;
    int cols;
    boolean *grid;
    uint16_t colour;
};

#endif