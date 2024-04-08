/**
 *	\file		logic.h
 *	\brief		Spécification de la couche Logic
 *	\author		Millian Lamiaux & Malo Garnier
 *	\date		8 avril 2024
 *	\version	1.0
 */

#ifndef LOGIC_H
#define LOGIC_H

/*
*****************************************************************************************
 *	\noop		I N C L U D E S   S P E C I F I Q U E S
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "ihm.h"

/*
*****************************************************************************************
 *	\noop		D E F I N I T I O N   DES   C O N S T A N T E S
 */

#define REDPLAYER 1
#define YELLOWPLAYER 2

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
 *	\fn			bool verifVictoire (int **matrix, int n, int m)
 *	\brief		Vérifie s'il y a une victoire dans la partie en cours
 *	\param		matrix : matrice d'entier comportant des 0, 1 ou 2
 *	\param		col : le nombre de colonnes de la matrice
 *	\param		ligne : le nombre de lignes de la matrice
 *  \return     boolean : True si victoire et false sinon.
 */
bool verifVictoire(int **matrix, int ligne, int col);

/**
 *	\fn			void afficherPlateau (int **matrix, int col, int ligne)
 *	\brief		Joue un jeton dans la colonne column
 *	\param		matrix : matrice d'entier comportant des 0, 1 ou 2
 *	\param		col : le nombre de colonnes de la matrice
 *	\param		ligne : le nombre de lignes de la matrice
 *  \param      column : l'entier de la colonne souhaitée
 *  \param      player : l'entier qui représente le joueur 1 ou 2.
 *	\return		int : 0 si l'insertion s'est bien déroulée, -1 sinon.
 */
int jouerJeton(int **matrix, int ligne, int column, int player);

#endif /* LOGIC_H */