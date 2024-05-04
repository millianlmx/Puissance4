#include <pthread.h>
#include "logic.h"
#include "colors.h"

// Mutex pour la synchronisation des joueurs
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Variable pour savoir quel joueur doit afficher le message
int tour = 1;

// Nombre de joueur
int nbPlayer;

// Stockage des joueurs
maillon_t *playerList = NULL;

// Plateau de jeu
plateau_t plateau;
int line;
int col;



void* thread_function(void* arg) {
    maillon_t *playerStorage = (maillon_t *) arg;
    int id = playerStorage->id;
    char *pseudo = playerStorage->pseudo;
    char *color = playerStorage->color;
    socket_t *socket = playerStorage->socket;

    while (1) {
        // Verrouillage du mutex pour éviter les accès concurrents
        pthread_mutex_lock(&mutex);

        // Si c'est le tour du joueur courant, afficher le message
        if (tour == id) {
            buffer_t buffer;
            memset(buffer, 0, sizeof(buffer_t));
            sprintf(buffer, "YOURTURN");
            envoyer(socket, buffer, NULL);

            // Attendre la réponse du joueur
            memset(buffer, 0, sizeof(buffer_t));
            recevoir(socket, buffer, NULL);

            // Récupération de la colonne jouée
            char *keyword = strtok(buffer, "::");
            int selectedCol = atoi(strtok(NULL, "::"));

            // Jouer le jeton
            jouerJeton(plateau, line, selectedCol, id);

            usleep(100000);

            // Envoyer la colonne jouée à tous les joueurs
            maillon_t *listExplorator = playerList;
            while (listExplorator != NULL)
            {
                memset(buffer, 0, sizeof(buffer_t));
                sprintf(buffer, "PLAY::%d::%d", id, selectedCol);
                envoyer(listExplorator->socket, buffer, NULL);
                listExplorator = listExplorator->suivant;
            }

            if(verifVictoire(plateau, line, col, nbPlayer))
            {
                // Envoyer la victoire à tous les joueurs
                listExplorator = playerList;
                while (listExplorator != NULL)
                {
                    memset(buffer, 0, sizeof(buffer_t));
                    sprintf(buffer, "VICTORY::%d", id);
                    envoyer(listExplorator->socket, buffer, NULL);
                    listExplorator = listExplorator->suivant;
                }

                break;
            }
            printf("ID : %d\n", id + 1);
            printf("nbPlayer : %d\n", nbPlayer + 1);
            tour = (id + 1) % (nbPlayer + 1); // Changer le tour pour l'autre joueur
            if (tour == 0) tour = 1;
            printf("Tour : %d\n", tour);
        }

        // Déverrouillage du mutex
        pthread_mutex_unlock(&mutex);

        // // Pause pour laisser le temps aux autres joueur de s'exécuter
        sleep(2);
    }
    return NULL;
}

int main()
{
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

    // Setting up the server
    char *ip = malloc(17 * sizeof(char));
    char *port = malloc(7 * sizeof(char));

    printf("Enter your ip address : ");
    fgets(ip, 17, stdin);

    printf("Enter your port number : ");
    fgets(port, 7, stdin);

    ip[strlen(ip) - 1] = '\0';
    port[strlen(port) - 1] = '\0';

    socket_t socket = creerSocketEcoute(ip, atoi(port)); 
    char colorLabels[12][12] = {"RED", "GREEN", "YELLOW", "BLUE", "MAGENTA", "CYAN", "BOLD_RED", "BOLD_GREEN", "BOLD_YELLOW", "BOLD_BLUE", "BOLD_MAGENTA", "BOLD_CYAN"};
    
    // Setting up players and playerList
    pthread_mutex_lock(&mutex);

    int i = 0;
    while (i < nbPlayer)
    {
        printf("Waiting for a new player...\n");
        socket_t *client = malloc(sizeof(socket_t));

        socket_t temp = accepterClt(socket);
        memcpy(client, &temp, sizeof(socket_t));

        // Setting up player colors
        int random = rand() % 12;
        while (strlen(colorLabels[random]) == 0) random = rand() % 12;
        
        char *label = colorLabels[random];
        
        // Getting the pseudo from client
        buffer_t buffer;
        memset(buffer, 0, sizeof(buffer_t));
        recevoir(client, buffer, NULL);

        colorLabels[random][0] = '\0';

        maillon_t *playerStorage = malloc(sizeof(maillon_t));
        printf("Creating player storage !\n");

        char *pseudo = malloc(sizeof(char) * strlen(buffer) + 1);
        strcpy(pseudo, buffer);
        playerStorage->socket = client;
        playerStorage->id = i + 1;
        playerStorage->colorIndex = random;
        playerStorage->pseudo = pseudo;
        playerStorage->suivant = playerList;

        playerList = playerStorage;

        pthread_create(&threads[i], NULL, thread_function, playerStorage);
        i++;
    }

    maillon_t *listExplorator = playerList;
    buffer_t bufferPlayerList;
    memset(bufferPlayerList, 0, sizeof(buffer_t));
    while (listExplorator != NULL)
    {
        sprintf(bufferPlayerList, "%s::%d::%d::%s", bufferPlayerList, listExplorator->id, listExplorator->colorIndex, listExplorator->pseudo);
        listExplorator = listExplorator->suivant;
    }

    printf("Player list : %s\n", bufferPlayerList);

    // send the list of players and game info to all players
    listExplorator = playerList;
    buffer_t buffer;
    while (listExplorator != NULL)
    {
        // Send game info
        printf("Sending game info to %s!\n", listExplorator->pseudo);
        memset(buffer, 0, sizeof(buffer_t));
        sprintf(buffer, "%d::%d::%d", line, col, nbPlayer);
        printf("Buffer game info : %s\n", buffer);
        envoyer(listExplorator->socket, buffer, NULL);
        printf("Game info sent to %s!\n", listExplorator->pseudo);
        
        usleep(100000);

        // Send player list
        envoyer(listExplorator->socket, bufferPlayerList, NULL);
        printf("Player list sent to %s!\n", listExplorator->pseudo);
        listExplorator = listExplorator->suivant;
    }

    pthread_mutex_unlock(&mutex);

    // Waiting for the end of the threads
    for (i = 0; i < nbPlayer; i++) {
        pthread_join(threads[i], NULL);
    }

    free(threads);
    return 0;
}

// void dialoguer(socket_t *passedClient)
// {
//     socket_t client = *passedClient;
    
//     buffer_t buffer;
//     memset(buffer, 0, sizeof(buffer_t));

//     plateau_t plateau;

//     recevoir(&client, buffer, NULL);

//     char *keyword = strtok(buffer, "::");
//     int line = atoi(strtok(NULL, "::"));
//     int col = atoi(strtok(NULL, "::"));

//     if (strcmp(keyword, "INIT") == 0)
//     {
//         printf("Starting game !\n");
//         printf("Lines : %d\n", line);
//         printf("Columns : %d\n", col);

//         printf("Creating the board...\n");
//         effacerShell();
//         plateau = creerPlateau(line, col);
//         afficherPlateau(plateau, line, col);

//         memset(buffer, 0, sizeof(buffer_t));
//         strcpy(buffer, "INITIALIZED");
//         envoyer(&client, buffer, NULL);

//         while (1)
//         {
//             memset(buffer, 0, sizeof(buffer_t));
//             recevoir(&client, buffer, NULL);
//             printf("Wait for the RED turn ...\n", buffer);
//             keyword = strtok(buffer, "::");

//             if(strcmp(keyword, "PLAY") == 0)
//             {
//                 char *selectedCol = strtok(NULL, "::");
//                 jouerJeton(plateau, line, atoi(selectedCol), REDPLAYER);
//                 effacerShell();
//                 afficherPlateau(plateau, line, col);
//                 memset(buffer, 0, sizeof(buffer_t));

//                 if(verifVictoire(plateau, line, col))
//                 {
//                     strcpy(buffer, "VICTORY::RED");
//                     envoyer(&client, buffer, NULL);
//                     break;
//                 }

//                 *selectedCol = '0' + askForColmun(col);
//                 jouerJeton(plateau, line, atoi(selectedCol), YELLOWPLAYER);
//                 effacerShell();
//                 afficherPlateau(plateau, line, col);
//                 sprintf(buffer, "PLAY::%s", selectedCol);
//                 envoyer(&client, buffer, NULL);
//                 memset(buffer, 0, sizeof(buffer_t));

//                 if(verifVictoire(plateau, line, col))
//                 {
//                     strcpy(buffer, "VICTORY::YELLOW");
//                     envoyer(&client, buffer, NULL);
//                     break;
//                 }

//             }

//         }

//         keyword = strtok(buffer, "::");
//         char *player = strtok(NULL, "::");
//         if(strcmp(player, "RED") == 0)
//         {
//             printf("Red player won !\n");
//         }
//         else
//         {
//             printf("Yellow player won !\n");
//         }

//         close(client.fd);
//         close(socket.fd);
//         effacerShell();
//         return 0;
//     }
//     else
//     {
//         printf("INIT not received\n");
//         memset(buffer, 0, sizeof(buffer_t));
//         strcpy(buffer, "ERROR");
//         effacerShell();
//         return 1;
//     }
// }