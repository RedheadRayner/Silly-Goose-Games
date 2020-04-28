#include <Arduino.h>
#include "tetrimino.h"

void Tetrimino::initialise(boolean *gridPass, int rowNum, int colNum, uint16_t tetColour, int multiplier)
{
    this->rows = rowNum;
    this->cols = colNum;
    int i, j, k, m, counter;
    Serial.println(sizeof(gridPass));
    
    this->grid = gridPass;
    this->colour = tetColour;
}

int Tetrimino:: blocksRight()
{
    int i, j;
    for (int i = 0; i < this->rows; ++i)
    {
        for (int j = 0; j < this->cols; j++)
        {
            if (booleanOfGrid(this->rows - 1 - i,this->cols - 1 - j))
            {
                return this->rows - i;
            }
        }
    }

    return this->rows - i;
}

int Tetrimino::  blocksDown()
{
      int i, j;
    for (int j = 0; j < this->cols; j++)
    {
        for (int i = 0; i < this->rows; ++i)
        {
            if (booleanOfGrid(this->rows - 1 - i,this->cols - 1 - j))
            {
                return this->cols - j;
            }
        }
    }

    return this->cols - j;
}

boolean Tetrimino:: booleanOfGrid(int row, int col){
return *grid + (row + this->cols*col);
}