all: client serveur

client:
# gcc -o client client.c logic.c session.c ihm.c data.c -lpthread -std=c99 -Wimplicit-function-declaration
	$(CCC) -o client client.c logic.c session.c ihm.c data.c -lpthread -std=c99 -Wimplicit-function-declaration -lwiringPi -lwiringPiDev

serveur:
# gcc -o serveur server.c logic.c session.c ihm.c data.c -lpthread -std=c99 -Wimplicit-function-declaration
	$(CCC) -o serveur server.c logic.c session.c ihm.c data.c -lpthread -std=c99 -Wimplicit-function-declaration

clean:
	rm -f client serveur