#include "tetriminoCollection.h"


using namespace std;

void TetriminoCollection::initialise(int scale)
{
    // the o tetrimino
    Tetrimino o_tetrimino;

    // the l tetrimino
    Tetrimino l_tetrimino;

    // the t tetrimino
    Tetrimino t_tetrimino;

    // the i tetrimino
    Tetrimino i_tetrimino;

    // the j tetrimino
    Tetrimino j_tetrimino;

    // the s tetrimino
    Tetrimino s_tetrimino;

    // the z tetrimino
    Tetrimino z_tetrimino;

    int oArray[4] = {
        1,
        1,
        1,
        1,
    };

    o_tetrimino.initialise(oArray, 2, Yellow, scale);

    int iArray[16] = {0, 0, 0, 0,
                      1, 1, 1, 1,
                      0, 0, 0, 0,
                      0, 0, 0, 0};

    i_tetrimino.initialise(iArray, 4, Blue, scale);

    int tArray[9] = {0, 1, 0,
                     1, 1, 1,
                     0, 0, 0};

    t_tetrimino.initialise(tArray, 3, Pink, scale);

    int lArray[9] = {0, 0, 1,
                     1, 1, 1,
                     0, 0, 0};

    l_tetrimino.initialise(lArray, 3, Orange, scale);

    int jArray[9] = {1, 0, 0,
                     1, 1, 1,
                     0, 0, 0};

    j_tetrimino.initialise(jArray, 3, DarkBlue, scale);

    int sArray[9] = {0, 1, 1,
                     1, 1, 0,
                     0, 0, 0};

    s_tetrimino.initialise(sArray, 3, Green, scale);

    int zArray[9] = {1, 1, 0,
                     0, 1, 1,
                     0, 0, 0};

    z_tetrimino.initialise(zArray, 3, Red, scale);

    this->tetriminoArray.push_back(o_tetrimino);
    this->tetriminoArray.push_back(l_tetrimino);
    this->tetriminoArray.push_back(t_tetrimino);
    this->tetriminoArray.push_back(i_tetrimino);
    this->tetriminoArray.push_back(j_tetrimino);
    this->tetriminoArray.push_back(s_tetrimino);
    this->tetriminoArray.push_back(z_tetrimino);
}

std::vector<Tetrimino> TetriminoCollection::getRandomSet()
{
    std::random_shuffle(tetriminoArray.begin(), tetriminoArray.end());
    return this->tetriminoArray;
}
