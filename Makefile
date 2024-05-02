all: client serveur

client:
	gcc -o client client.c logic.c session.c ihm.c data.c -lpthread

serveur:
	gcc -o serveur registration_server.c logic.c session.c ihm.c data.c -DSERVEUR

clean:
	rm -f client serveur