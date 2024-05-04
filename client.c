#include <pthread.h>
#include "logic.h"

char colors[12][10] = {RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, BOLD_RED, BOLD_GREEN, BOLD_YELLOW, BOLD_BLUE, BOLD_MAGENTA, BOLD_CYAN};

void reset(generic *quoi);
int askForColumn(int col);
void senregistrer(char *serverIP, int serverPort, char *color, socket_t *sockEch);
int client(void *arg);

int main()
{
    buffer_t buffer;
    char *serverIP = malloc(17 * sizeof(char));
    char *serverPort = malloc(7 * sizeof(char));

    printf("Enter the game server IP : ");
    fgets(serverIP, 17, stdin);
    printf("Enter the game server port : ");
    fgets(serverPort, 7, stdin);

    serverIP[strlen(serverIP) - 1] = '\0';
    serverPort[strlen(serverPort) - 1] = '\0';

    char *color = malloc(12 * sizeof(char));
    socket_t *sockEch = malloc(sizeof(socket_t));

    senregistrer(serverIP, atoi(serverPort), color, sockEch);

    char *pseudo = malloc(200 * sizeof(char));
    printf("Enter your pseudo : ");
    fgets(pseudo, 200, stdin);
    pseudo[strlen(pseudo) - 1] = '\0';

    // Sending pseudo
    memset(buffer, 0, sizeof(buffer_t));
    strcpy(buffer, pseudo);
    envoyer(sockEch, pseudo, NULL);

    // Receiving the game infos
    memset(buffer, 0, sizeof(buffer_t));
    recevoir(sockEch, buffer, NULL);

    printf("Receiving game infos...\n");

    int line = atoi(strtok(buffer, "::"));
    int col = atoi(strtok(NULL, "::"));
    char *tmp = strtok(NULL, "::");
    tmp[1] = '\0';
    int nbPlayers = atoi(tmp);

    printf("Game infos received !\n");
    printf("Line : %d\n", line);
    printf("Column : %d\n", col);
    printf("Nb players : %d\n", nbPlayers);
    memset(buffer, 0, sizeof(buffer_t));

    plateau_t plateau = creerPlateau(line, col);
    printf("Plateau created !\n");

    int i = 0;
    maillon_t *playerList = NULL;

    // receiving the player data in the following format : id::colorIndex::pseudo
    memset(buffer, 0, sizeof(buffer_t));
    recevoir(sockEch, buffer, NULL);

    while (i < nbPlayers)
    {
        printf("Storing player %d infos...\n", i + 1);
        maillon_t *temp = malloc(sizeof(maillon_t));
        memset(temp, 0, sizeof(player_t));

        char *pseudo = malloc(200 * sizeof(char));
        char *color = malloc(11 * sizeof(char));
        memset(pseudo, 0, 200 * sizeof(char));
        memset(color, 0, 11 * sizeof(char));

        // Creation of the player and adding it to the player list
        if(i == 0) temp->id = atoi(strtok(&buffer[2], "::"));
        else temp->id = atoi(strtok(NULL, "::"));
        temp->colorIndex = atoi(strtok(NULL, "::"));
        strcpy(color, colors[temp->colorIndex]);
        temp->color = color;
        strcpy(pseudo, strtok(NULL, "::"));
        temp->pseudo = pseudo;
        temp->suivant = playerList;

        printf("Player details : %d::%d::%s\n", temp->id, temp->colorIndex, temp->pseudo);
        
        playerList = temp;

        i++;
    }

    afficherPlateau(plateau, line, col, colors);

    while (true)
    {
        memset(buffer, 0, sizeof(buffer_t));
        recevoir(sockEch, buffer, NULL);

        char *keyword = strtok(buffer, "::");
        if(strcmp(keyword, "YOURTURN") == 0)
        {
            int selectedCol = askForColmun(col);
            memset(buffer, 0, sizeof(buffer_t));
            sprintf(buffer, "PLAYED::%d", selectedCol);
            envoyer(sockEch, buffer, NULL);
        }
        else if(strcmp(keyword, "PLAY") == 0)
        {
            int playerId = atoi(strtok(NULL, "::"));
            int selectedCol = atoi(strtok(NULL, "::"));
            jouerJeton(plateau, line, selectedCol, playerId);
            effacerShell();
            afficherPlateau(plateau, line, col, colors);
        }
        else if(strcmp(keyword, "VICTORY") == 0)
        {
            int playerId = atoi(strtok(NULL, "::"));
            maillon_t *temp = playerList;
            while(temp != NULL)
            {
                if(temp->id == playerId)
                {
                    printf("Player %s won !\n", temp->pseudo);
                    break;
                }
                temp = temp->suivant;
            }
            break;
        }
        
    }
    return 0;
}

// int client(void *arg)
// {
//     player_t *temp = (player_t *)arg;
//     char *serverIP = temp->ip;
//     char *serverPort = temp->port;

//     socket_t sockEch = connecterClt2Srv(serverIP, serverPort);
//     buffer_t buffer;
//     memset(buffer, 0, sizeof(buffer_t));

//     char *line = malloc(3 * sizeof(char));
//     reset(line);
//     char *col = malloc(3 * sizeof(char));
//     reset(col);

//     printf("Enter the number of lines : ");
//     fgets(line, 3, stdin);
//     printf("Enter the number of columns : ");
//     fgets(col, 3, stdin);
//     line[strlen(line) - 1] = '\0';
//     col[strlen(col) - 1] = '\0';

//     int lineNumber = atoi(line);
//     int colNumber = atoi(col);

//     sprintf(buffer, "INIT::%d::%d", lineNumber, colNumber);

//     envoyer(&sockEch, buffer, NULL);
//     memset(buffer, 0, sizeof(buffer_t));

//     recevoir(&sockEch, buffer, NULL); // INITIALIZED
//     effacerShell();
//     printf("Game initialized !\n");

//     plateau_t plateau = creerPlateau(lineNumber, colNumber);
//     afficherPlateau(plateau, lineNumber, colNumber);

//     while (1)
//     {
//         int selectedCol = askForColmun(colNumber);
//         jouerJeton(plateau, lineNumber, selectedCol, REDPLAYER);

//         effacerShell();
//         afficherPlateau(plateau, lineNumber, colNumber);
//         printf("Waiting for the yellow turn...\n");

//         sprintf(buffer, "PLAY::%d", selectedCol);
//         envoyer(&sockEch, buffer, NULL);

//         memset(buffer, 0, sizeof(buffer_t));
//         recevoir(&sockEch, buffer, NULL);

//         char *keyword = strtok(buffer, "::");

//         if (strcmp(keyword, "VICTORY") == 0)
//         {
//             char *player = strtok(NULL, "::");
//             if (strcmp(player, "RED") == 0)
//             {
//                 printf("Red player won !\n");
//             }
//             else
//             {
//                 printf("Yellow player won !\n");
//             }
//             break;
//         }
//         else
//         {
//             selectedCol = atoi(strtok(NULL, "::"));
//             jouerJeton(plateau, lineNumber, selectedCol, YELLOWPLAYER);
//             effacerShell();
//             afficherPlateau(plateau, lineNumber, colNumber);
//             if (verifVictoire(plateau, lineNumber, colNumber))
//             {
//                 printf("Yellow player won !\n");
//                 break;
//             }
//         }
//     }
//     effacerShell();
// }

void senregistrer(char *serverIP, int serverPort, char *color, socket_t *sockEch)
{
    printf("Connecting to the server...\n");
    socket_t temp = connecterClt2Srv(serverIP, serverPort);

    memcpy(sockEch, &temp, sizeof(socket_t));
}

void reset(generic *quoi)
{
    memset(quoi, 0, sizeof(generic));
}

int askForColmun(int col)
{
    char temp[4];
    printf("Enter column number (1-%d): ", col);
    fgets(temp, 4, stdin);
    return atoi(temp) - 1;
}