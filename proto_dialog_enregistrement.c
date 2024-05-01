#include "logic.h"

#ifdef SERVEUR
int main()
{
    socket_t socket = creerSocketEcoute("192.168.1.15", 50000);

    socket_t client = accepterClt(socket);

    player_t *player = malloc(sizeof(player_t));
    memset(player, 0, sizeof(player));

    player->ip = malloc(15*sizeof(char));
    player->port = malloc(5*sizeof(char));
    player->pseudo = malloc(201*sizeof(char));

    buffer_t buffer;
    memset(buffer, 0, sizeof(buffer));

    recevoir(&client, buffer, NULL);

    str2Player(buffer, player);

    printf("Player IP : %s\n", player->ip);
    printf("Player port : %s\n", player->port);
    printf("Player pseudo : %s\n", player->pseudo);
}
#else
int main()
{
    socket_t sockEch = connecterClt2Srv("192.168.1.15", 50000);

    // Enter your ip address and port number
    char *ip = malloc(15*sizeof(char));
    char *port = malloc(5*sizeof(char));
    char *pseudo = malloc(201*sizeof(char));
    printf("Enter the ip address : ");
    scanf("%s", ip);
    printf("Enter the port number : ");
    scanf("%s", port);
    printf("Enter your pseudo : ");
    scanf("%s", pseudo);

    player_t *player = (player_t *)malloc(sizeof(player_t));
    memset(player, 0, sizeof(player));
    player->ip = ip;
    player->port = port;
    player->pseudo = pseudo;

    buffer_t buffer;
    memset(buffer, 0, sizeof(buffer));
    player2Str(buffer, player);
    printf("Message envoyé : %s\n", buffer);

    envoyer(&sockEch, buffer, NULL);
}
#endif