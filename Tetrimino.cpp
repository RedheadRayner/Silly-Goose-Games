#include <Arduino.h>
#include "tetrimino.h"

void Tetrimino::initialise(boolean **gridPass, int rowNum, int colNum, uint16_t tetColour, int multiplier)
{

    this->rows = rowNum;
    this->cols = colNum;
    int i, j, k, m, counter;
    boolean multipliedGrid[rowNum][colNum];
    counter = 0;
    for (i = 0; i < rowNum; i++)
    {
        for (j = 0; j < colNum; j++)
        {
            for (m = 0; m < multiplier; m++)
            {
                for (k = 0; k < multiplier; k++)
                {
                    multipliedGrid[i + m][j + k] = gridPass[i][j];
                    Serial.println(counter);
                    counter++;
                }
            }
        }
    }
    boolean *ptr[rowNum];
    for (size_t i = 0; i < rowNum; ++i)
    {
        ptr[i] = gridPass[i];
    }
    this->grid = ptr;
    this->colour = tetColour;
}