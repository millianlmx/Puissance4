all: client serveur

client:
	gcc -o client client.c logic.c session.c ihm.c data.c -lpthread -Wall -Wextra

serveur:
	gcc -o serveur server.c logic.c session.c ihm.c data.c -lpthread -Wall -Wextra

clean:
	rm -f client serveur