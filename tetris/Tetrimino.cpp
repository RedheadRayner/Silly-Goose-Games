
#include "tetrimino.h"

void Tetrimino::initialise(int gridPass[], int dim, uint16_t tetColour, int scaleDisplay)
{
    this->rows = dim;
    this->cols = dim;
    startGrid = new int[dim * dim];
    currentGrid = new int[dim * dim];
    this->colour = tetColour;

        for (int j = 0; j < this->cols; j++)
        {
                for (int i = 0; i < this->rows; ++i)
    {
            startGrid[i + dim * j] = gridPass[i + dim * j];
            currentGrid[i + dim * j] = gridPass[i + dim * j];
        }
    }

    
}

// Spawns the tetrimino piece
void Tetrimino::spawn(){
  for (int j = 0; j < this->cols; j++)
        {
                for (int i = 0; i < this->rows; ++i)
    {currentGrid
            [i + cols * j] = startGrid[i + cols * j];
        }
    }
}

// The number of blocks to the right until it hits the playfield right-hand side
int Tetrimino::blocksRight()
{
    int i, j;
    for (int i = 0; i < this->rows; ++i)
    {
        for (int j = 0; j < this->cols; j++)
        {
            if (booleanOfGrid(this->rows - 1 - i, this->cols - 1 - j))
            {
                return this->rows - i;
            }
        }
    }

    return this->rows - i;
}

// The number of blocks to the left until it hits the playfield left-hand side
int Tetrimino::blocksLeft()
{
    int i, j;
    for (int i = 0; i < this->rows; ++i)
    {
        for (int j = 0; j < this->cols; j++)
        {
            if (booleanOfGrid( i,  j))
            {
                return i;
            }
        }
    }

    return  i;
}

// The number of blocks to downward until it hits the playfield bottom
int Tetrimino::blocksDown()
{
    int i, j;
    for (int j = 0; j < this->cols; j++)
    {
        for (int i = 0; i < this->rows; ++i)
        {
            if (booleanOfGrid(this->rows - 1 - i, this->cols - 1 - j))
            {
                return this->cols - j;
            }
        }
    }

    return this->cols - j;
}

boolean Tetrimino::booleanOfGrid(int row, int col)
{
    int gridVal = this->currentGrid[row + this->cols * col];
    if (gridVal == 1)
    {
        return true;
    }
    return false;
}

boolean Tetrimino::booleanOfStartGrid(int row, int col)
{
    int gridVal = this->startGrid[row + this->cols * col];
    if (gridVal == 1)
    {
        return true;
    }
    return false;
}

//0°) i=wy + x
//90°) i=w(w-1) + y - wx
//180°) i= w^2 - 1 - wy - x
//270°) i= w - 1 - y +wx
// Rotates the tetrimino grid
void Tetrimino::rotateGrid()
{
int rotatedGrid[this->cols * this->rows];

     for (int i = 0; i < this->rows; ++i)
    {
        for (int j = 0; j < this->cols; j++)
        {

            rotatedGrid[i + this->cols * j] = currentGrid[( this->cols - j - 1) + this->cols *i ];
        }
    }
    for (int i = 0; i < this->rows; ++i)
    {
        for (int j = 0; j < this->cols; j++)
        {

            currentGrid[i +  this->cols * j] = rotatedGrid[i + this->cols * j];
        }
    }
}

// Undoes rotation
void Tetrimino::unrotateGrid()
{
int unrotatedGrid[this->cols * this->rows];

     for (int i = 0; i < this->rows; ++i)
    {
        for (int j = 0; j < this->cols; j++)
        {

            unrotatedGrid[( this->cols - j - 1) + this->cols *i] = currentGrid[i + this->cols * j ];
        }
    }
    for (int i = 0; i < this->rows; ++i)
    {
        for (int j = 0; j < this->cols; j++)
        {

            currentGrid[i +  this->cols * j] = unrotatedGrid[i + this->cols * j];
        }
    }
}
