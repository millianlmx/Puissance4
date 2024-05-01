all: client serveur

client:
	gcc -o client game.c logic.c session.c ihm.c data.c

serveur:
	gcc -o serveur game.c logic.c session.c ihm.c data.c -DSERVEUR

clean:
	rm -f client serveur