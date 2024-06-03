#include <pthread.h>
#include "logic.h"

#define ASSERVER 1
#define ASCLIENT 2

char publicKeyWord[20] = "puissance4";
char colorLabels[13][12] = {"", "RED", "GREEN", "YELLOW", "BLUE", "MAGENTA", "CYAN", "BOLD_RED", "BOLD_GREEN", "BOLD_YELLOW", "BOLD_BLUE", "BOLD_MAGENTA", "BOLD_CYAN"};
char colors[13][10] = {"", RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, BOLD_RED, BOLD_GREEN, BOLD_YELLOW, BOLD_BLUE, BOLD_MAGENTA, BOLD_CYAN};

void reset(generic *quoi);
int askForColumn(int col);

int main()
{
    key_t publicKey;
    char filename[8] = "serveur"; 
    word2Key(5000, &publicKey, filename);

    printf("Connecting to the server...\n");
    connecterClt2Srv(publicKey);
    printf("Connecting to the server...\n");

    pid_t clientPID = getpid();
    char keyword[20];
    sprintf(keyword, "%d", clientPID);

    // Création de la clé IPC
    key_t privateKey;
    memset(filename, 0, sizeof(filename));
    strcpy(filename, "client"); 
    word2Key(clientPID, &privateKey, filename);

    // Receiving the game infos
    buffer_t buffer;
    memset(buffer, 0, sizeof(buffer_t));
    recevoir(privateKey, buffer, ASSERVER);

    printf("Receiving game infos...\n");

    int line = atoi(strtok(buffer, "::"));
    int col = atoi(strtok(NULL, "::"));
    int nbPlayers = atoi(strtok(NULL, "::"));

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
    recevoir(privateKey, buffer, ASSERVER);

    while (i < nbPlayers)
    {
        printf("Storing player %d infos...\n", i + 1);
        maillon_t *temp = malloc(sizeof(maillon_t));
        memset(temp, 0, sizeof(maillon_t));

        player_t player;
        memset(&player, 0, sizeof(player_t));

        char *pseudo = malloc(200 * sizeof(char));
        memset(pseudo, 0, 200 * sizeof(char));

        // Creation of the player and adding it to the player list
        if(i == 0) player.id = atoi(strtok(&buffer[2], "::"));
        else player.id = atoi(strtok(NULL, "::"));
        player.colorIndex = atoi(strtok(NULL, "::"));
        strcpy(player.pseudo, strtok(NULL, "::"));
        temp->suivant = playerList;

        memcpy(&temp->player, &player, sizeof(player_t));

        printf("Player details : %d::%d::%s\n", temp->player.id, temp->player.colorIndex, temp->player.pseudo);
        
        playerList = temp;

        i++;
    }

    afficherPlateau(plateau, line, col, colors);

    while (true)
    {
        memset(buffer, 0, sizeof(buffer_t));
        recevoir(privateKey, buffer, ASSERVER);

        char *keyword = strtok(buffer, "::");
        if(strcmp(keyword, "YOURTURN") == 0)
        {
            int selectedCol = askForColumn(col);
            memset(buffer, 0, sizeof(buffer_t));
            sprintf(buffer, "PLAYED::%d", selectedCol);
            envoyer(privateKey, buffer, ASCLIENT);
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
                if(temp->player.id == playerId)
                {
                    printf("Player %s won !\n", temp->player.pseudo);
                    break;
                }
                temp = temp->suivant;
            }
            break;
        }
    }
    return 0;
}

void reset(generic *quoi)
{
    memset(quoi, 0, sizeof(generic));
}

int askForColumn(int col)
{
    char temp[4];
    printf("Enter column number (1-%d): ", col);
    fgets(temp, 4, stdin);
    return atoi(temp) - 1;
}