#include <Arduino.h>
#include "tetrimino.h"

void Tetrimino::initialise(boolean **gridPass, int rowNum, int colNum, uint16_t tetColour, int multiplier)
{

    this->rows = rowNum;
    this->cols = colNum;
    this->multiRows = rowNum * multiplier;
    this->multiCols = colNum * multiplier;
    int i, j, k, m, counter;

    boolean *array_data_multi = new boolean[rowNum * multiplier * (cols * multiplier)];
    boolean **array_multi = new boolean *[rowNum * multiplier];
    for (int i = 0; i < rowNum * multiplier; ++i)
    {
        array_multi[i] = array_data_multi + (cols * multiplier) * i;
    }

    for (i = 0; i < rowNum; i++)
    {
        for (j = 0; j < colNum; j++)
        {
            for (m = 0; m < multiplier; m++)
            {
                for (k = 0; k < multiplier; k++)
                {
                    array_multi[i + m][j + k] = gridPass[i][j];
                }
            }
        }
    }

    boolean *array_data = new boolean[rowNum * colNum];
    boolean **array = new boolean *[rowNum];
    for (int i = 0; i < rowNum; ++i)
    {
        array[i] = array_data + colNum * i;
    }

    for (i = 0; i < rowNum; i++)
    {
        for (j = 0; j < colNum; j++)
        {
            array[i][j] = gridPass[i][j];

            counter++;
        }
    }
    this->grid = array;
    this->multiGrid = array;
    this->colour = tetColour;
}

int Tetrimino:: blocksRight()
{
    int i, j;
    for (int i = 0; i < this->rows; ++i)
    {
        for (int j = 0; j < this->cols; j++)
        {
            if (this->multiGrid[this->rows - 1 - i][this->cols - 1 - j])
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
            if (this->multiGrid[this->rows - 1 - i][this->cols - 1 - j])
            {
                return this->cols - j;
            }
        }
    }

    return this->cols - j;
}