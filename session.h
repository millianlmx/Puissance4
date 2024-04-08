/**
 *	\file		session.h
 *	\brief		Spécification de la couche Session
 *	\author		Samir El Khattabi
 *	\date		3 mars 2023
 *	\version	1.0
 */
#ifndef SESSION_H
#define SESSION_H
/*
*****************************************************************************************
 *	\noop		I N C L U D E S   S P E C I F I Q U E S
 */
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
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
/**
 *	\def		PAUSE(msg)
 *	\brief		Macro-fonction qui affiche msg et attend une entrée clavier  
 */
#define PAUSE(msg)	printf("%s [Appuyez sur entrée pour continuer]", msg); getchar();
/*
*****************************************************************************************
 *	\noop		S T R C T U R E S   DE   D O N N E E S
 */
/**
 *	\struct		socket
 *	\brief		Définition de la structure de données socket
 *	\note		Ce type est composé du fd de la socket, du mode (connecté/non)
 *				et des adresses applicatives (locale/distante)
 */
struct socket {
	int fd;							/**< numéro de la socket créée			*/
	int mode;						/**< mode connecté/non : STREAM/DGRAM	*/
	struct sockaddr_in addrLoc;		/**< adresse locale de la socket 		*/
	struct sockaddr_in addrDst;		/**< adresse distante de la socket 		*/
};
/**
 *	\typedef	socket_t
 *	\brief		Définition du type de données socket_t
 */
typedef struct socket socket_t; 
/*
*****************************************************************************************
 *	\noop		P R O T O T Y P E S   DES   F O N C T I O N S
 */
/**
 *	\fn			void adr2struct (struct sockaddr_in *addr, char *adrIP, short port)
 *	\brief		Transformer une adresse au format humain en structure SocketBSD
 *	\param		addr : structure d'adressage BSD d'une socket INET
 *	\param		adrIP : adresse IP de la socket créée
 *	\param		port : port de la socket créée
 *	\note		Le domaine dépend du mode choisi (TCP/UDP)
 *	\result		paramètre *adr modifié
 */
void adr2struct (struct sockaddr_in *addr, char *adrIP, short port);
/**
 *	\fn			socket_t creerSocket (int mode)
 *	\brief		Création d'une socket de type DGRAM/STREAM
 *	\param		mode : mode connecté (STREAM) ou non (DGRAM)
 *	\result		socket créée selon le mode choisi
 */
socket_t creerSocket (int mode);
/**
 *	\fn			socket_t creerSocketAdr (int mode, char *adrIP, short port)
 *	\brief		Création d'une socket de type DGRAM/STREAM
 *	\param		mode : adresse IP de la socket créée
 *	\param		adrIP : adresse IP de la socket créée
 *	\param		port : port de la socket créée
 *	\result		socket créée dans le domaine choisi avec l'adressage fourni
 */
socket_t creerSocketAdr (int mode, char *adrIP, short port);
/**
 *	\fn			creerSocketEcoute (char *adrIP, short port)
 *	\brief		Création d'une socket d'écoute avec l'adressage fourni en paramètre
 *	\param		adrIP : adresse IP du serveur à mettre en écoute
 *	\param		port : port TCP du serveur à mettre en écoute
 *	\result		socket créée avec l'adressage fourni en paramètre et dans un état d'écoute
 *	\note		Le domaine est nécessairement STREAM
 */
socket_t creerSocketEcoute (char *adrIP, short port);
/**
 *	\fn			socket_t accepterClt (const socket_t sockEcoute)
 *	\brief		Acceptation d'une demande de connexion d'un client
 *	\param		sockEcoute : socket d'écoute pour réception de la demande
 *	\result		socket (dialogue) connectée par le serveur avec un client
 */
socket_t accepterClt (const socket_t sockEcoute);
/**
 *	\fn			socket_t connecterClt2Srv (char *adrIP, short port)
 *	\brief		Crétaion d'une socket d'appel et connexion au seveur dont
 *				l'adressage est fourni en paramètre
 *	\param		adrIP : adresse IP du serveur à connecter
 *	\param		port : port TCP du serveur à connecter
 *	\result		socket connectée au serveur fourni en paramètre
 */
socket_t connecterClt2Srv (char *adrIP, short port);

#endif /* SESSION_H */
