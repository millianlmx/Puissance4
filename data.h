/**
 *	\file		data.h
 *	\brief		Spécification de la couche Data Representation
 *	\author		Samir El Khattabi
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
#include "session.h"
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
    char *ip;
    char *port;
    char *pseudo;
} player_t;

/**
 *	\typedef	maillon_t
 *	\brief		stockage des joueurs sous forme de liste chaînée
 */
typedef struct
{
    socket_t *socket;
    int id;
    int colorIndex;
    char *pseudo;
    char *color;
    struct maillon_t *suivant;
} maillon_t;
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
 *	\fn			void envoyer(socket_t *sockEch, generic quoi, pFct serial, ...)
 *	\brief		Envoi d'une requête/réponse sur une socket
 *	\param 		sockEch : socket d'échange à utiliser pour l'envoi
 *	\param 		quoi : requête/réponse à serialiser avant l'envoi
 *	\param 		serial : pointeur sur la fonction de serialisation d'une requête/réponse
 *	\note		si le paramètre serial vaut NULL alors quoi est une chaîne de caractères
 *	\note		Si le mode est DGRAM, l'appel nécessite en plus l'adresse IP et le port.
 *	\result		paramètre sockEch modifié pour le mode DGRAM
 */
void envoyer(socket_t *sockEch, generic quoi, pFct serial);
/**
 *	\fn			void recevoir(socket_t *sockEch, generic quoi, pFct deSerial)
 *	\brief		Réception d'une requête/réponse sur une socket
 *	\param 		sockEch : socket d'échange à utiliser pour la réception
 *	\param 		quoi : requête/réponse reçue après dé-serialisation du buffer de réception
 *	\param 		deSerial : pointeur sur la fonction de dé-serialisation d'une requête/réponse
 *	\note		si le paramètre deSerial vaut NULL alors quoi est une chaîne de caractères
 *	\result		paramètre quoi modifié avec le requête/réponse reçue
 */
void recevoir(socket_t *sockEch, generic quoi, pFct deSerial);

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
