#ifndef TET_H
#define TET_H

class Tetrimino {

  public:
    virtual void initialise(boolean **, int, int, uint16_t, int );
    int rows;
    int cols;
    boolean **grid;
    uint16_t colour;
};

#endif