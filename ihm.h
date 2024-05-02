/**
 *	\file		ihm.h
 *	\brief		Spécification de la couche IHM
 *	\author		Millian Lamiaux & Malo Garnier
 *	\date		8 avril 2024
 *	\version	1.0
 */

#ifndef IHM_H
#define IHM_H

/*
*****************************************************************************************
 *	\noop		I N C L U D E S   S P E C I F I Q U E S
 */
#include <stdio.h>
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
 *	\noop		S T R C T U R E S   DE   D O N N E E S
 */

/*
*****************************************************************************************
 *	\noop		P R O T O T Y P E S   DES   F O N C T I O N S
 */
/**
 *	\fn			void afficherPlateau (int **matrix, int col, int ligne)
 *	\brief		Affiche la matrice du Puissance 4 au format humain dans le Shell
 *	\param		matrix : matrice d'entier comportant des 0, 1 ou 2
 *	\param		col : le nombre de colonnes de la matrice
 *	\param		ligne : le nombre de lignes de la matrice
 *	\note		Ne pas donner une matrice trop grande sous peine de dégrader l'expérience utilisateur
 */
void afficherPlateau(int **matrix, int col, int ligne);

/**
 *	\fn			void effacerShell ()
 *	\brief		Effacele contenu du Shell
 */
void effacerShell();

#endif /* IHM_H */