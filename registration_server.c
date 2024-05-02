#include "logic.h"

int main()
{
    printf("Mise en place du serveur d'enregistrement !\n");

    char *ip = malloc(17 * sizeof(char));
    char *port = malloc(7 * sizeof(char));

    printf("Enter your ip address : ");
    fgets(ip, 17, stdin);

    printf("Enter your port number : ");
    fgets(port, 7, stdin);

    ip[strlen(ip) - 1] = '\0';
    port[strlen(port) - 1] = '\0';

    printf("Server IP : %s\n", ip);
    printf("Server Port : %s\n", port);

    printf("Server is running...\n");
    socket_t socket = creerSocketEcoute(ip, port);

    maillon_t *playerList = NULL; 
    while (1)
    {
        printf("Waiting for a new player...\n");
        socket_t client = accepterClt(socket);

        player_t *player = malloc(sizeof(player_t));
        memset(player, 0, sizeof(player));

        player->ip = malloc(15 * sizeof(char));
        player->port = malloc(5 * sizeof(char));
        player->pseudo = malloc(201 * sizeof(char));

        buffer_t buffer;
        memset(buffer, 0, sizeof(buffer));

        recevoir(&client, buffer, NULL);

        str2Player(buffer, player);

        maillon_t *newPlayer = malloc(sizeof(maillon_t));
        memset(newPlayer, 0, sizeof(maillon_t));
        newPlayer->player = player;
        newPlayer->suivant = playerList;

        playerList = newPlayer;

        memset(buffer, 0, sizeof(buffer_t));
        strcpy(buffer, "REGISTERED");

        envoyer(&client, buffer, NULL);

        close(client.fd);
    }
}
