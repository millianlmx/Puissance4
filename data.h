/**
 *	\file		data.h
 *	\brief		Spécification de la couche Data Representation
 *	\author		Millian Lamiaux
 *	\date		3 mars 2023
 *	\version	1.0
 */
#ifndef DATA_H
#define DATA_H

/*
*****************************************************************************************
 *	\noop		I N C L U D E S   S P E C I F I Q U E S
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <stdbool.h>
#include "colors.h"

/*
*****************************************************************************************
 *	\noop		D E F I N I T I O N   DES   C O N S T A N T E S
 */

/**
 *	\def		MAX_BUFFER
 *	\brief		taille d'un buffer_t d'émission/réception
 */
#define MAX_BUFFER	1024

/*
*****************************************************************************************
 *	\noop		D E F I N I T I O N   DES   M A C R O S
 */
/**
 *	\def		CHECK(sts, msg)
 *	\brief		Macro-fonction qui vérifie que sts est égal -1 (cas d'erreur : sts==-1) 
 *				En cas d'erreur, il y a affichage du message adéquat et fin d'exécution  
 */
#define CHECK(sts, msg) if ((sts)==-1) {perror(msg); exit(-1);}

/*
*****************************************************************************************
 *	\noop		S T R C T U R E S   DE   D O N N E E S
 */

/**
 *	\typedef	buffer_t
 *	\brief		chaîne de caractères à émettre/recevoir
 */
typedef char buffer_t[MAX_BUFFER];

/**
 *	\typedef	plateau_t
 *	\brief		matrice de nombre entiers
 */
typedef int ** plateau_t;

/**
 *	\typedef	player_t
 *	\brief		nombre entier représentant un joueur
 */
typedef struct
{
    char keyWord[7];
    char pseudo[200];
    int id;
    int colorIndex;
    int clientPID;
} player_t;

typedef struct {
	long mtype;
	buffer_t mtext;
} message_t;

struct maillon_t 
{
    player_t player;
    struct maillon_t *suivant;
};

/**
 *	\typedef	maillon_t
 *	\brief		stockage des joueurs sous forme de liste chaînée
 */
typedef struct maillon_t maillon_t;

/**
 *	\typedef	generic
 *	\brief		type de données générique : requêtes/réponses
 */
typedef void * generic;

/**
 *	\typedef	pFct
 *	\brief		pointer sur fonction générique à 2 parametres génériques
 */
typedef void (*pFct) (generic, generic);

/*
*****************************************************************************************
 *	\noop		P R O T O T Y P E S   DES   F O N C T I O N S
 */

/**
 *	\fn			void envoyer(key_t key, char *data, int mode)
 *	\brief		Envoi d'une requête/réponse sur une boite au lettre avec la clé IPC
 *	\param 		key : clé IPC de la boite au lettre
 *	\param 		data : requête/réponse à envoyer
 *  \param      mode : entier pour spécifer le "canal" de réception
 */
void envoyer(key_t key, char *data, int mode);

/**
 *	\fn			void recevoir(key_t key, char *data)
 *	\brief		Réception d'une requête/réponse sur une boite au lettre avec la clé IPC
 *	\param 		key : clé IPC de la boite au lettre
 *	\param 		data : requête/réponse à envoyer
 *  \param      mode : entier pour spécifer le "canal" de réception
 *	\result		entier > 0 si envoie ok, -1 sinon
 */
void recevoir(key_t key, char *data, int mode);

/**
 *	\fn			void str2Player(buffer_t buffer, generic quoi)
 *	\brief		Dé-sérialisation d'une chaîne de caractères en structure player_t
 *	\param 		buffer : chaîne de caractères à dé-sérialiser
 *	\param 		quoi : structure player_t à remplir
 */
void str2Player(buffer_t buffer, generic quoi);

/**
 *	\fn			void player2Str(buffer_t buffer, generic quoi)
 *	\brief		Serialisation d'une structure player_t en chaîne de caractères
 *	\param 		buffer : chaîne de caractères résultante de la sérialisation
 *	\param 		quoi : structure player_t à sérialiser
 */
void player2Str(buffer_t buffer, generic quoi);

#endif /* DATA_H */