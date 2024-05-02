/**
 *	\file		data.h
 *	\brief		Spécification de la couche Data Representation
 *	\author		Samir El Khattabi
 *	\date		3 mars 2023
 *	\version	1.0
 */

#include "data.h"

void envoyer(socket_t *sockEch, generic quoi, pFct serial)
{
    if (serial != NULL)
    {
        char buffer[1024];
        // Serialize the data
        serial(buffer, quoi);

        // Send the data over the socket
        CHECK(send(sockEch->fd, buffer, strlen(buffer), 0), "Can't send the data !")
    }
    else
    {
        // Treat quoi as a string
        const char *str = (const char *)quoi;
        // Send the string over the socket
        CHECK(send(sockEch->fd, str, strlen(str), 0), "Can't send the string !")
    }
}

void recevoir(socket_t *sockEch, generic quoi, pFct deSerial)
{
    if (deSerial != NULL)
    {
        // Receive the data
        char buffer[1024];
        CHECK(recv(sockEch->fd, buffer, sizeof(buffer), 0), "Can't receive the data !")
        // Deserialize the data
        deSerial(buffer, quoi);
    }
    else
    {
        // Treat quoi as a string
        char *str = (char *)quoi;
        // Receive the string
        CHECK(recv(sockEch->fd, str, 1024, 0), "Can't receive the string !")
    }
}

void player2Str(char *buffer, generic quoi)
{
    player_t *player = (player_t *)quoi;
    sprintf(buffer, "%s::%s::%s", player->ip, player->port, player->pseudo);
}

void str2Player(buffer_t buffer, generic quoi)
{
    player_t *player = (player_t *)quoi;

    char *ip = strtok(buffer, "::");
    char *port = strtok(NULL, "::");
    char *pseudo = strtok(NULL, "::");

    strcpy(player->ip, ip);
    strcpy(player->port, port);
    strcpy(player->pseudo, pseudo);
}