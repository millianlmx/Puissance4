#include "logic.h"
#include "colors.h"

int main()
{
    int n = 6, m = 7;
    int **matrix = (int **)malloc(n * sizeof(int *));

    for (int i = 0; i < n; i++)
    {
        matrix[i] = (int *)malloc(m * sizeof(int));
    }

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            matrix[i][j] = 0;
        }
    }

    int player = REDPLAYER;
    int column = 3;
    jouerJeton(matrix, n, column, player);
    afficherPlateau(matrix, n, m);
    printf("Victory: %d\n", verifVictoire(matrix, n, m));
    return 0;
}