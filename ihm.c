#include "ihm.h"
#include "colors.h"

void afficherPlateau(int **matrix,  int ligne, int col)
{
    char separator[] = "##########";

    char *figure[] = {
        "██████╗",
        "██╔═══██╗",
        "██║   ██║",
        "██║   ██║",
        "╚██████╔╝",
        "╚═════╝"};

    char *emptyFigure[] = {
        "       ",
        "         ",
        "         ",
        "         ",
        "         ",
        "       ",
    };

    char *delimiter[] = {
        "###",
        "#",
        "#",
        "#",
        "#",
        "###",
    };

    char *baseDelimiter[] = {
        "##",
        "#",
        "#",
        "#",
        "#",
        "##",
    };

    for (int i = 0; i < ligne; i++)
    { // for each line
        for (int l = 0; l < 7; l++)
        { // for line of figure
            for (int j = 0; j < col; j++)
            { // for each token
                if (l == 0)
                    printf("%s", separator);
                else
                {
                    const char *color = matrix[i][j] == 1 ? RED : YELLOW;
                    printf("%s%s%s%s%s", j == 0 ? baseDelimiter[l - 1] : delimiter[l - 1], color, matrix[i][j] != 0 ? figure[l - 1] : emptyFigure[l - 1], WHITE, j == col - 1 ? baseDelimiter[l - 1] : "");
                }

                if (l == 0 && j == col - 1)
                {
                    printf("#");
                }
            }
            printf("\n");
        }
    }

    for (int l = 0; l < col; l++)
        printf("%s", separator);
    printf("#\n");
}