#include "logic.h"

bool verifVictoire(int **matrix, int lignes, int colonnes)
{
    for (int l = REDPLAYER; l <= YELLOWPLAYER; l++)
    {
        for (int i = 0; i < lignes; i++)
        {
            for (int j = 0; j < colonnes; j++)
            {
                if (matrix[i][j] == l)
                {
                    if (i + 3 < colonnes && matrix[i + 1][j] == l && matrix[i + 2][j] == l && matrix[i + 3][j] == l)
                    {
                        return true;
                    }
                    if (j + 3 < colonnes && matrix[i][j + 1] == l && matrix[i][j + 2] == l && matrix[i][j + 3] == l)
                    {
                        return true;
                    }
                    if (i + 3 < lignes && j + 3 < colonnes && matrix[i + 1][j + 1] == l && matrix[i + 2][j + 2] == l && matrix[i + 3][j + 3] == l)
                    {
                        return true;
                    }
                    if (i + 3 < lignes && j - 3 >= 0 && matrix[i + 1][j - 1] == l && matrix[i + 2][j - 2] == l && matrix[i + 3][j - 3] == l)
                    {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

int jouerJeton(int **matrix, int lignes, int column, int player)
{
    int i;
    for (i = lignes - 1; i >= 0; i--)
    {
        if (matrix[i][column] == 0)
        {
            matrix[i][column] = player;
            return 0;
        }
    }
    return -1;
}
