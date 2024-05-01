#include "logic.h"

#ifdef SERVEUR
int main()
{
    socket_t socket = creerSocketEcoute("192.168.1.15", 50000);

    socket_t client = accepterClt(socket);

    buffer_t buffer;
    memset(buffer, 0, sizeof(buffer));

    recevoir(&client, buffer, NULL);

    printf("Message reçu : %s\n", buffer);

}
#else
int main()
{
    socket_t sockEch = connecterClt2Srv("192.168.1.15", 50000);

    buffer_t buffer;
    memset(buffer, 0, sizeof(buffer));

    fgets(buffer, sizeof(buffer), stdin);
    buffer[strlen(buffer) - 1] = '\0';

    envoyer(&sockEch, buffer, NULL);
}
#endif