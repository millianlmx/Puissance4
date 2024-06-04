/**
 *	\file		session.c
 *	\brief		Implémentation de la couche Session
 *	\author		Millian Lamiaux
 *	\date		3 mars 2023
 *	\version	1.0
 */

#include "session.h"
#define ASSERVER 1
#define ASCLIENT 2

pid_t serverPID;

void handleSignal(int signal)
{
    if (signal == SIGINT) {
        printf("Caught SIGINT signal\n");
        // send SIGUSR1 to server
        kill(serverPID, SIGUSR1);
    }
    else if (signal == SIGUSR1) {
        printf("A player has disconnected\n");
        exit(0);
    }
}

void word2Key (int pid, key_t *key, char *filepath) 
{
    CHECK(*key = ftok(filepath, pid), "Error key creation !")
}


maillon_t* accepterClt (key_t publicKey, maillon_t *playerList, char colorLabels[12][12])
{
    buffer_t buffer;
    printf("Receiving registration request...\n");
    recevoir(publicKey, buffer, ASCLIENT); // connect::clientpid::pseudo
    printf("Received : %s\n", buffer);

    strtok(buffer, "::");
    int clientPID = atoi(strtok(NULL, "::"));
    char *pseudo = strtok(NULL, "::");

    // Création de la clé IPC
    key_t privateKey;
    char filename[7] = "client"; 
    word2Key(clientPID, &privateKey, filename);

    // Setting up player colors
    int random = rand() % 12;
    while (strlen(colorLabels[random]) == 0) random = rand() % 12;

     colorLabels[random][0] = '\0';

    printf("Player %s connected !\n", pseudo);
    
    player_t player;
    memset(&player, 0, sizeof(player_t));

    strcpy(player.keyWord, filename);
    strcpy(player.pseudo, pseudo);
    player.colorIndex = random;
    player.clientPID = clientPID;

    maillon_t *playerStorage = malloc(sizeof(maillon_t));
    memcpy(&playerStorage->player, &player, sizeof(player_t));
    playerStorage->suivant = playerList;


    memset(buffer, 0, sizeof(buffer_t));
    sprintf(buffer, "registration::ok::%d", getpid());
    envoyer(privateKey, buffer, ASSERVER);

    return playerStorage;
}

int connecterClt2Srv (key_t publicKey)
{
    // prompt pseudo to user
    char pseudo[200];
    printf("Enter your pseudo : ");
    fgets(pseudo, 200, stdin);
    pseudo[strlen(pseudo) - 1] = '\0';

    pid_t clientPID = getpid();

    buffer_t buffer;
    memset(buffer, 0, sizeof(buffer_t));
    sprintf(buffer, "connect::%d::%s", clientPID, pseudo);
    printf("Sending registration request...\n");
    printf("%s\n", buffer);
    envoyer(publicKey, buffer, ASCLIENT);

    // Création de la clé IPC
    key_t privateKey;
    char filename[8] = "client"; 
    word2Key(clientPID, &privateKey, filename);

    printf("Receiving registration response...\n");
    memset(buffer, 0, sizeof(buffer_t));
    recevoir(privateKey, buffer, ASSERVER); // registration::ok::serverPID

    char *keyword = strtok(buffer, "::");
    char *value = strtok(NULL, "::");
    serverPID = atoi(strtok(NULL, "::"));

    // handle CTRL+C signal using sigaction and send SIGUSR1 to server using serverPID

    struct sigaction sa;
    sa.sa_handler = handleSignal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);

    if (strcmp(keyword, "registration") == 0 && strcmp(value, "ok") == 0) {
        return 0;
    } else {
        return -1;
    }
}

void fermerLesBAL(maillon_t *playerList)
{
    maillon_t *listExplorator = playerList;
    while (listExplorator != NULL)
    {
        key_t playerKey;
        char filename[8] = "client"; 
        word2Key(listExplorator->player.clientPID, &playerKey, filename);

        msgctl(msgget(playerKey, 0666 | IPC_CREAT), IPC_RMID, NULL);
        listExplorator = listExplorator->suivant;
    }

    key_t publicKey;
    char filename[8] = "serveur"; 
    word2Key(5000, &publicKey, filename);
    msgctl(msgget(publicKey, 0666 | IPC_CREAT), IPC_RMID, NULL);
}