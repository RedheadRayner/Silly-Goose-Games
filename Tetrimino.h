#ifndef TET_H
#define TET_H

class Tetrimino {

  public:
    initialise(boolean **, int, int, uint16_t, int );
    virtual int blocksRight();
    virtual int blocksDown();
    int rows;
    int cols;
    int multiRows;
    int multiCols;
    boolean **grid;
    boolean **multiGrid;
    uint16_t colour;
};

#endif