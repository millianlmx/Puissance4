#include <pthread.h>
#include "logic.h"

void reset(generic *quoi);
int askForColumn(int col);
int senregistrer(char *serverIP, int serverPort, char *listeningIP, char *listeningPort);
int serveur(void* arg);
int client(void* arg);

int main() {
    char *serverIP = malloc(17 * sizeof(char));
    char *serverPort = malloc(7 * sizeof(char));
    char *listeningIP = malloc(17 * sizeof(char));
    char *listeningPort = malloc(7 * sizeof(char));

    printf("Enter the registration server IP : ");
    fgets(serverIP, 17, stdin);
    printf("Enter the registration server port : ");
    fgets(serverPort, 7, stdin);
    printf("Enter your ip address : ");
    fgets(listeningIP, 17, stdin);
    printf("Enter your port number : ");
    fgets(listeningPort, 7, stdin);

    serverIP[strlen(serverIP) - 1] = '\0';
    serverPort[strlen(serverPort) - 1] = '\0';
    listeningIP[strlen(listeningIP) - 1] = '\0';
    listeningPort[strlen(listeningPort) - 1] = '\0';

    if(senregistrer(serverIP, atoi(serverPort), listeningIP, listeningPort))
    {
        // launch a serveur thread and a client thread
        pthread_t client, serveur;

        player_t *listening = malloc(sizeof(player_t));
        memset(listening, 0, sizeof(listening));
        listening->ip = listeningIP;
        listening->port = listeningPort;
        pthread_create(&serveur, NULL, serveur, listening);

        player_t *server = malloc(sizeof(player_t));
        memset(server, 0, sizeof(server));
        server->ip = serverIP;
        server->port = serverPort;
        pthread_create(&client, NULL, client, server);

        pthread_join(serveur, NULL);
        pthread_join(client, NULL);
    }
    else
    {
        printf("Registration failed !\n");
    }

    return 0;
}

int serveur(void* arg)
{
    player_t *temp = (player_t *)arg;
    char *listeningIP = temp->ip;
    char *listeningPort = temp->port;

    socket_t socket = creerSocketEcoute(listeningIP, atoi(listeningPort));
    socket_t client = accepterClt(socket);
    buffer_t buffer;
    memset(buffer, 0, sizeof(buffer_t));

    plateau_t plateau;

    recevoir(&client, buffer, NULL);

    char *keyword = strtok(buffer, "::");
    int line = atoi(strtok(NULL, "::"));
    int col = atoi(strtok(NULL, "::"));

    if (strcmp(keyword, "INIT") == 0)
    {
        printf("Starting game !\n");
        printf("Lines : %d\n", line);
        printf("Columns : %d\n", col);

        printf("Creating the board...\n");
        effacerShell();
        plateau = creerPlateau(line, col);
        afficherPlateau(plateau, line, col);

        memset(buffer, 0, sizeof(buffer_t));
        strcpy(buffer, "INITIALIZED");
        envoyer(&client, buffer, NULL);

        while (1)
        {
            memset(buffer, 0, sizeof(buffer_t));
            recevoir(&client, buffer, NULL);
            printf("Wait for the RED turn ...\n", buffer);
            keyword = strtok(buffer, "::");
            if(strcmp(keyword, "PLAY") == 0)
            {
                char *selectedCol = strtok(NULL, "::");
                jouerJeton(plateau, line, atoi(selectedCol), REDPLAYER);
                effacerShell();
                afficherPlateau(plateau, line, col);
                memset(buffer, 0, sizeof(buffer_t));

                if(verifVictoire(plateau, line, col))
                {
                    strcpy(buffer, "VICTORY::RED");
                    envoyer(&client, buffer, NULL);
                    break;
                }

                *selectedCol = '0' + askForColmun(col);
                jouerJeton(plateau, line, atoi(selectedCol), YELLOWPLAYER);
                effacerShell();
                afficherPlateau(plateau, line, col);
                sprintf(buffer, "PLAY::%s", selectedCol);
                envoyer(&client, buffer, NULL);
                memset(buffer, 0, sizeof(buffer_t));

                if(verifVictoire(plateau, line, col))
                {
                    strcpy(buffer, "VICTORY::YELLOW");
                    envoyer(&client, buffer, NULL);
                    break;
                }

            }

        }

        keyword = strtok(buffer, "::");
        char *player = strtok(NULL, "::");
        if(strcmp(player, "RED") == 0)
        {
            printf("Red player won !\n");
        }
        else
        {
            printf("Yellow player won !\n");
        }

        close(client.fd);
        close(socket.fd);
        effacerShell();
        return 0;
    }
    else
    {
        printf("INIT not received\n");
        memset(buffer, 0, sizeof(buffer_t));
        strcpy(buffer, "ERROR");
        effacerShell();
        return 1;
    }
}

int client(void* arg)
{
    player_t *temp = (player_t*)arg;
    char *serverIP = temp->ip; 
    char *serverPort = temp->port;
    
    socket_t sockEch = connecterClt2Srv(serverIP, serverPort);
    buffer_t buffer;
    memset(buffer, 0, sizeof(buffer_t));

    char *line = malloc(3 * sizeof(char));
    reset(line);
    char *col = malloc(3 * sizeof(char));
    reset(col);

    printf("Enter the number of lines : ");
    fgets(line, 3, stdin);
    printf("Enter the number of columns : ");
    fgets(col, 3, stdin);
    line[strlen(line) - 1] = '\0';
    col[strlen(col) - 1] = '\0';

    int lineNumber = atoi(line);
    int colNumber = atoi(col);

    sprintf(buffer, "INIT::%d::%d", lineNumber, colNumber);

    envoyer(&sockEch, buffer, NULL);
    memset(buffer, 0, sizeof(buffer_t));

    recevoir(&sockEch, buffer, NULL); // INITIALIZED
    effacerShell();
    printf("Game initialized !\n");

    plateau_t plateau = creerPlateau(lineNumber, colNumber);
    afficherPlateau(plateau, lineNumber, colNumber);
    
    while (1)
    {
        int selectedCol = askForColmun(colNumber);
        jouerJeton(plateau, lineNumber, selectedCol, REDPLAYER);

        effacerShell();
        afficherPlateau(plateau, lineNumber, colNumber);
        printf("Waiting for the yellow turn...\n");

        sprintf(buffer, "PLAY::%d", selectedCol);
        envoyer(&sockEch, buffer, NULL);

        memset(buffer, 0, sizeof(buffer_t));
        recevoir(&sockEch, buffer, NULL);

        char *keyword = strtok(buffer, "::");

        if(strcmp(keyword, "VICTORY") == 0)
        {
            char *player = strtok(NULL, "::");
            if(strcmp(player, "RED") == 0)
            {
                printf("Red player won !\n");
            }
            else
            {
                printf("Yellow player won !\n");
            }
            break;
        }
        else
        {
            selectedCol = atoi(strtok(NULL, "::"));
            jouerJeton(plateau, lineNumber, selectedCol, YELLOWPLAYER);
            effacerShell();
            afficherPlateau(plateau, lineNumber, colNumber);
            if(verifVictoire(plateau, lineNumber, colNumber))
            {
                printf("Yellow player won !\n");
                break;
            }
        }
    }
    effacerShell();
}

int senregistrer(char *serverIP, int serverPort, char *listeningIP, char *listeningPort)
{
    printf("Connecting to the server...\n");
    printf("Server IP : %s\n", serverIP);
    printf("Server Port : %d\n", serverPort);
    socket_t sockEch = connecterClt2Srv(serverIP, serverPort);

    char *pseudo = malloc(201 * sizeof(char));

    printf("Enter your pseudo : ");
    fgets(pseudo, 201, stdin);

    listeningIP[strlen(listeningIP) - 1] = '\0';
    listeningPort[strlen(listeningPort) - 1] = '\0';
    pseudo[strlen(pseudo) - 1] = '\0';

    player_t *player = (player_t *)malloc(sizeof(player_t));
    memset(player, 0, sizeof(player));
    player->ip = listeningIP;
    player->port = listeningPort;
    player->pseudo = pseudo;

    buffer_t buffer;
    memset(buffer, 0, sizeof(buffer_t));
    player2Str(buffer, player);

    envoyer(&sockEch, buffer, NULL);
    memset(buffer, 0, sizeof(buffer_t));

    recevoir(&sockEch, buffer, NULL);
    return strcmp(buffer, "REGISTERED") == 0;
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