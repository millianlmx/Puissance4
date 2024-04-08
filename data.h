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
void envoyer(socket_t *sockEch, generic quoi, pFct serial, ...);
/**
 *	\fn			void recevoir(socket_t *sockEch, generic quoi, pFct deSerial)
 *	\brief		Réception d'une requête/réponse sur une socket
 *	\param 		sockEch : socket d'échange à utiliser pour la réception
 *	\param 		quoi : requête/réponse reçue après dé-serialisation du buffer de réception
 *	\param 		deSerial : pointeur sur la fonction de dé-serialisation d'une requête/réponse
 *	\note		si le paramètre deSerial vaut NULL alors quoi est une chaîne de caractères
 *	\result		paramètre quoi modifié avec le requête/réponse reçue
 *				paramètre sockEch modifié pour le mode DGRAM
 */
void recevoir(socket_t *sockEch, generic quoi, pFct deSerial);

#endif /* DATA_H */
