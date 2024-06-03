#include "logic.h"

bool verifVictoire(int **matrix, int lignes, int colonnes, int nbPlayer)
{
    for (int l = 1; l <= nbPlayer; l++)
    {
        for (int i = 0; i < lignes; i++)
        {
            for (int j = 0; j < colonnes; j++)
            {
                if (matrix[i][j] == l)
                {
                    if (i + 3 < lignes && matrix[i + 1][j] == l && matrix[i + 2][j] == l && matrix[i + 3][j] == l)
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

int jouerJeton(plateau_t matrix, int lignes, int column, int player)
{
    for (int i = lignes - 1; i >= 0; i--)
    {
        if (matrix[i][column] == 0)
        {
            matrix[i][column] = player;
            return 0;
        }
    }
    return -1;
}

plateau_t creerPlateau(int n, int m) 
{
    plateau_t matrix = (plateau_t)malloc(n * sizeof(int *));

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

    return matrix;
}

// void jouerPartie()
// {

//     struct winsize w;
//     ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

//     int raw_lines = w.ws_row;
//     int raw_columns = w.ws_col;
//     int p4_lines, p4_columns;

//     if (raw_lines % 7 > 0 || raw_columns % 10 > 0)
//     {
//         p4_lines = raw_lines / 7;
//         p4_columns = raw_columns / 10;
//     }
//     else
//     {
//         p4_lines = raw_lines / 7 - 1;
//         p4_columns = raw_columns / 10 - 1;
//     }

//     int n, m;

//     printf("Entrez la taille du plateau (ligne colonne) max (%d %d): ", p4_lines, p4_columns);
//     scanf("%d %d", &n, &m);

//     if (n > p4_lines || m > p4_columns)
//     {
//         n = p4_lines;
//         m = p4_columns;
//     }

//     plateau_t plateau = creerPlateau(n, m);

//     int player = REDPLAYER;

//     while (verifVictoire(plateau, n, m) == false)
//     {
//         printf("\033[H\033[J");
//         afficherPlateau(plateau, n, m);
//         printf("Joueur %d, entrez la colonne ou vous voulez jouer: ", player);
//         int column;
//         scanf("%d", &column);
//         if (jouerJeton(plateau, n, column - 1, player) == -1)
//         {
//             printf("Colonne pleine, veuillez en choisir une autre\n");
//             continue;
//         }
//         else
//         {
//             player = player == REDPLAYER ? YELLOWPLAYER : REDPLAYER;
//         }
//     }

//     afficherPlateau(plateau, n, m);
//     printf("Joueur %d a gagne\n", player == REDPLAYER ? YELLOWPLAYER : REDPLAYER);

// }