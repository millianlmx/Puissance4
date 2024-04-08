all: game

game:
	gcc -o game game.c logic.c session.c ihm.c data.c