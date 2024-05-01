#include <stdio.h>
#include <time.h>

int main()
{

    printf("\033[H\033[J");

    char *figure[] = {
        "██████╗",
        "██╔═══██╗",
        "██║   ██║",
        "██║   ██║",
        "╚██████╔╝",
        "╚═════╝",
    };

    int i = 0;
    while (i < 30)
    {   for(int j = 0; j < i; j++)
            printf("\n");
        for (int l = 0; l < 6; l++) {
            if(l == 0 || l == 5)
                printf(" %s\n", figure[l]);
            else
                printf("%s\n", figure[l]);
        }

        usleep(175000);

        // clear shell
        printf("\033[H\033[J");

        i++;
    }
    

    return 0;
}

// #include <ncurses.h>
// #include <locale.h>
// #include <wchar.h>

// int main() {
//     // Initialiser le support UTF-8 dans libncurses
//     WINDOW* current = initscr();

//     waddstr(current, setlocale(LC_ALL, ""));
//     waddstr(current, "\n");

//     keypad(current, TRUE);
//     noecho();
    
//     if (!has_colors()) {
//         endwin();
//         printf("Your terminal does not support color\n");
//         exit(1);
//     }
    
//     start_color();
//     init_pair(1, COLOR_BLACK, COLOR_WHITE);
//     attron(COLOR_PAIR(1));

//     // Activer le support UTF-8 pour la fenêtre standard
//     if (wattr_set(current, A_NORMAL, NULL, NULL) == -1) {
//         endwin();
//         printf("Error setting window attributes\n");
//         exit(1);
//     }

//     // Afficher le caractère "█" (U+2588)
//     //const char *block = " █"; // UTF-8 encoding for "█"
//     // waddstr(current, "\342\234\224\n");

//     wchar_t c = 0xA1C;
//     char wstr[] = { '█', L'\0' };
//     waddstr(current, wstr);
//     putwchar(c);

//     // Rafraîchir l'écran et attendre une entrée utilisateur
//     refresh();
//     getch();

//     // Nettoyer et quitter
//     attroff(COLOR_PAIR(1));
//     endwin();
//     return 0;
// }
