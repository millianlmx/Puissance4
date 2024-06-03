#include <pthread.h>
#include "logic.h"

#define ASSERVER 1
#define ASCLIENT 2

char *publicKeyWord;

// Mutex pour la synchronisation des joueurs
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Variable pour savoir quel joueur doit afficher le message
int tour = 1;
int victory = 0;

// Nombre de joueur
int nbPlayer;

// Stockage des joueurs
maillon_t *playerList = NULL;

// Plateau de jeu
plateau_t plateau;
int line;
int col;

char colorLabels[12][12] = {"RED", "GREEN", "YELLOW", "BLUE", "MAGENTA", "CYAN", "BOLD_RED", "BOLD_GREEN", "BOLD_YELLOW", "BOLD_BLUE", "BOLD_MAGENTA", "BOLD_CYAN"};
char colors[12][10] = {RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, BOLD_RED, BOLD_GREEN, BOLD_YELLOW, BOLD_BLUE, BOLD_MAGENTA, BOLD_CYAN};

void handleSignalServer(int signal)
{
    if (signal == SIGINT) {
        printf("Caught SIGINT signal\n");
        // send SIGUSR1 to all players
        maillon_t *listExplorator = playerList;
        while (listExplorator != NULL)
        {
            kill(listExplorator->player.clientPID, SIGUSR1);
            listExplorator = listExplorator->suivant;
        }
        exit(0);
    }
    else if (signal == SIGUSR1) {
        printf("A player has disconnected\n");
        maillon_t *listExplorator = playerList;
        while (listExplorator != NULL)
        {
            kill(listExplorator->player.clientPID, SIGUSR1);
            listExplorator = listExplorator->suivant;
        }
        exit(0);
    }
}

void* thread_function(void* arg) {
    player_t *playerStorage = (player_t *) arg;
    int id = playerStorage->id;
    pid_t clientPID = playerStorage->clientPID;
    char *pseudo = playerStorage->pseudo;
    // char *color = colors[playerStorage->colorIndex];
    char *keyWord = playerStorage->keyWord;

    // printing player details
    printf("Player details : %d::%d::%s::%s\n", id, clientPID, pseudo, keyWord);

    // Création de la clé IPC
    key_t privateKey;
    char filename[8] = "client"; 
    word2Key(clientPID, &privateKey, filename);

    while (1) {
        // Verrouillage du mutex pour éviter les accès concurrents
        pthread_mutex_lock(&mutex);

        if(victory) 
        {
            tour = (id + 1) % (nbPlayer + 1); // Changer le tour pour l'autre joueur
            // Déverrouillage du mutex
            pthread_mutex_unlock(&mutex);
            break;
        }

        // Si c'est le tour du joueur courant, afficher le message
        if (tour == id) {
            buffer_t buffer;
            memset(buffer, 0, sizeof(buffer_t));
            strcpy(buffer, "YOURTURN");
            printf("Sending YOURTURN to %s !\n", pseudo);
            envoyer(privateKey, buffer, ASSERVER);
            printf("Sended\n");


            // Attendre la réponse du joueur
            memset(buffer, 0, sizeof(buffer_t));
            recevoir(privateKey, buffer, ASCLIENT);
            printf("Received : %s\n", buffer);

            // Récupération de la colonne jouée
            strtok(buffer, "::");
            int selectedCol = atoi(strtok(NULL, "::"));

            // Jouer le jeton
            jouerJeton(plateau, line, selectedCol, id);

            // Envoyer la colonne jouée à tous les joueurs
            maillon_t *listExplorator = playerList;
            while (listExplorator != NULL)
            {
                key_t playerKey;
                char filename[8] = "client"; 
                word2Key(listExplorator->player.clientPID, &playerKey, filename);

                memset(buffer, 0, sizeof(buffer_t));
                sprintf(buffer, "PLAY::%d::%d", id, selectedCol);
                envoyer(playerKey, buffer, ASSERVER);

                listExplorator = listExplorator->suivant;
            }

            sleep(1);

            if(verifVictoire(plateau, line, col, nbPlayer))
            {
                printf("Player %s won !\n", pseudo);

                // Envoyer la victoire à tous les joueurs
                listExplorator = playerList;
                while (listExplorator != NULL)
                {
                    printf("Sending victory to %s !\n", listExplorator->player.pseudo);

                    key_t playerKey;
                    char filename[8] = "client"; 
                    word2Key(listExplorator->player.clientPID, &playerKey, filename);

                    memset(buffer, 0, sizeof(buffer_t));
                    sprintf(buffer, "VICTORY::%d", id);

                    envoyer(playerKey, buffer, ASSERVER);
                    printf("Victory sent to %s !\n", listExplorator->player.pseudo);

                    listExplorator = listExplorator->suivant;
                }

                printf("Setting victory and turn for other threads...\n");
                victory = 1;
                tour = (id + 1) % (nbPlayer + 1); // Changer le tour pour l'autre joueur
                // Déverrouillage du mutex
                pthread_mutex_unlock(&mutex);
                break;
            }
            tour = (id + 1) % (nbPlayer + 1); // Changer le tour pour l'autre joueur
            if (tour == 0) tour = 1;
        }

        // Déverrouillage du mutex
        pthread_mutex_unlock(&mutex);

        // Pause pour laisser le temps aux autres joueur de s'exécuter
        // sleep(1);
    }
    printf("Exiting player thread ...\n");
    return NULL;
}

int main(int argc, char *argv[])
{
    // handle CTRL+C signal using sigaction and send SIGUSR1
    struct sigaction action;
    action.sa_handler = handleSignalServer;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGUSR1, &action, NULL);

    publicKeyWord = argv[0];
    pthread_t* threads;

    // Seting up the game settings
    printf("How many player for this game : ");
    char temp[3];
    fgets(temp, 3, stdin);
    nbPlayer = atoi(temp);
    memset(temp, 0, 3);

    printf("Enter the number of line of the grid : ");
    fgets(temp, 3, stdin);
    line = atoi(temp);
    memset(temp, 0, 3);

    printf("Enter the number of column of the grid : ");
    fgets(temp, 3, stdin);
    col = atoi(temp);
    memset(temp, 0, 3);

    plateau = creerPlateau(line, col);

    threads = malloc(nbPlayer * sizeof(pthread_t));

    key_t publicKey;
    char filename[8] = "serveur"; 
    word2Key(5000, &publicKey, filename);
    
    // Setting up players and playerList
    pthread_mutex_lock(&mutex);

    buffer_t bufferPlayerList;
    memset(bufferPlayerList, 0, sizeof(buffer_t));

    int i = 0;
    while (i < nbPlayer)
    {
        printf("Waiting for a new player...\n");
        playerList = accepterClt(publicKey, playerList, colorLabels);
        playerList->player.id = i + 1;
        printf("Player %s accepted !\n", playerList->player.pseudo);
        sprintf(bufferPlayerList, "%s::%d::%d::%s", bufferPlayerList, playerList->player.id, playerList->player.colorIndex, playerList->player.pseudo);

        player_t *player = malloc(sizeof(player_t));
        player->id = playerList->player.id;
        player->clientPID = playerList->player.clientPID;
        strcpy(player->pseudo, playerList->player.pseudo);
        player->colorIndex = playerList->player.colorIndex;
        strcpy(player->keyWord, playerList->player.keyWord);

        pthread_create(&threads[i], NULL, thread_function, player);
        i++;
    }


    printf("Player list : %s\n", bufferPlayerList);

    // send the list of players and game info to all players
    maillon_t *listExplorator = playerList;
    buffer_t buffer;
    while (listExplorator != NULL)
    {
        key_t playerKey;
        char filename[8] = "client"; 
        word2Key(listExplorator->player.clientPID, &playerKey, filename);

        // Send game info
        printf("Sending game info to %s!\n", listExplorator->player.pseudo);
        memset(buffer, 0, sizeof(buffer_t));
        sprintf(buffer, "%d::%d::%d", line, col, nbPlayer);
        printf("Buffer game info : %s\n", buffer);
        envoyer(playerKey, buffer, ASSERVER);
        printf("Game info sent to %s!\n", listExplorator->player.pseudo);
        
        usleep(100000);

        // Send player list
        envoyer(playerKey, bufferPlayerList, ASSERVER);
        printf("Player list sent to %s!\n", listExplorator->player.pseudo);
        listExplorator = listExplorator->suivant;
    }

    pthread_mutex_unlock(&mutex);
    printf("Game is starting !\n");

    // Waiting for the end of the threads
    for (i = 0; i < nbPlayer; i++) {
        pthread_join(threads[i], NULL);
    }

    free(threads);
    return 0;
}