/**
 *	\file		session.h
 *	\brief		Spécification de la couche Session
 *	\author		Millian Lamiaux
 *	\date		3 mars 2023
 *	\version	1.0
 */
#ifndef SESSION_H
#define SESSION_H_
#define _GNU_SOURCE
/*
*****************************************************************************************
 *	\noop		I N C L U D E S   S P E C I F I Q U E S
 */
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include "data.h"
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
 *	\noop		P R O T O T Y P E S   DES   F O N C T I O N S
 */
/**
 *	\fn			void word2Key (int pid, key_t *key, char *filepath) 
 *	\brief		Transformer une chaine de caractère en clé IPC
 *	\param		word : chaine de caractère à transformer
 *	\param		key : clé IPC résultante
 *	\result		la clé IPC résultante de la transformation
 */
void word2Key (int pid, key_t *key, char *filepath);

/**
 *	\fn			void creerBAL (key_t *key, int *balId)
 *	\brief		Création d'une boite aux lettres
 *	\param		key : clé IPC
  *	\param		balId : identifiant de la boite aux lettre
 */
void creerBAL (key_t *key, int *balId);

/**
 *	\fn			maillon_t* accepterClt (key_t publicKey, maillon_t *playerList, char colorLabels[12][12])
 *	\brief		Acceptation d'une demande de connexion d'un client, résultant sur la creation, le stockage du joueur grâce aux écahnges sur une boite au lettres privée.
 *	\param		publicKey : clé IPC
 *  \param      mutux : pointer vers le mutex de la playerList
 *  \param      playerList : pointer vers la liste des joueurs
 *	\result		0 si création ok, -1 sinon.
 */
maillon_t* accepterClt (key_t publicKey, maillon_t *playerList, char colorLabels[12][12]);

/**
 *	\fn			int connecterClt2Srv (int publicBal)
 *	\brief		Dialogue de communication avec le serveur sur la boite au lettre principale pour initier une communication sur une boite lettre privée pour s'enregistrer
 *				l'adressage est fourni en paramètre
 *	\param		bal : id de la boite au lettre commune
 *	\result		0 si enregistrement ok, -1 sinon.
 */
int connecterClt2Srv (int publicBal);

/**
 *	\fn			void fermerLesBAL(maillon_t *playerList)
 *	\brief		Permet de fermer toutes les boites aux lettres des joueurs et du serveur.
 *	\param		playerList : liste des joueurs 
 */
void fermerLesBAL(maillon_t *playerList);

#endif /* SESSION_H */
