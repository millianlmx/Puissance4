/**
 *	\file		data.h
 *	\brief		Spécification de la couche Data Representation
 *	\author		Millian Lamiaux
 *	\date		3 mars 2023
 *	\version	1.0
 */

#include "data.h"

void envoyer(key_t key, char *data, int mode)
{
    int msgid = msgget(key, 0666 | IPC_CREAT);
    CHECK(msgid, "msgget")
    message_t msg;
    msg.mtype = mode;
    strcpy(msg.mtext, data);
    msgsnd(msgid, &msg, MAX_BUFFER, 0);
}

void recevoir(key_t key, char *data, int mode)
{
    int msgid = msgget(key, 0666 | IPC_CREAT);
    CHECK(msgid, "msgget")
    message_t msg;
    msgrcv(msgid, &msg, MAX_BUFFER, mode, 0);
    strcpy(data, msg.mtext);
}

void player2Str(buffer_t buffer, generic quoi)
{
    player_t *player = (player_t *)quoi;
    sprintf(buffer, "%s::%s::%d::%d", player->keyWord, player->pseudo, player->id, player->colorIndex);
}

void str2Player(buffer_t buffer, generic quoi)
{
    player_t *player = (player_t *)quoi;

    char *keyWord = strtok(buffer, "::");
    char *pseudo = strtok(NULL, "::");
    player->id = atoi(strtok(NULL, "::"));
    player->colorIndex = atoi(strtok(NULL, "::"));

    strcpy(player->pseudo, pseudo);
    strcpy(player->keyWord, keyWord);
}