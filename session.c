/**
 *	\file		session.c
 *	\brief		Spécification de la couche Session
 *	\author		Millian Lamiaux
 *	\date		3 mars 2023
 *	\version	1.0
 */


#include "session.h"

#define CHECK(sts, msg) \
    if ((sts) == -1)    \
    {                   \
        perror(msg);    \
        exit(-1);       \
    }
#define PAUSE(msg)                                         \
    printf("%s [Appuyez sur entrée pour continuer]", msg); \
    getchar();

struct socket
{
    int fd;                     /**< numéro de la socket créée			*/
    int mode;                   /**< mode connecté/non : STREAM/DGRAM	*/
    struct sockaddr_in addrLoc; /**< adresse locale de la socket 		*/
    struct sockaddr_in addrDst; /**< adresse distante de la socket 		*/
};

typedef struct socket socket_t;

void adr2struct(struct sockaddr_in *addr, char *adrIP, short port)
{
    // Initialisation de la structure d'adressage
    memset(addr, 0, sizeof(*addr));

    // Domaine AF\_INET
    addr->sin_family = AF_INET;

    // Transformation de l'adresse IP en binaire
    inet_pton(AF_INET, adrIP, &addr->sin_addr);

    // Port
    addr->sin_port = htons(port);
}

socket_t creerSocket(int mode)
{
    socket_t sock;
    CHECK(sock.fd = socket(AF_INET, mode, 0), "Erreur lors de la création de la socket !");
    sock.mode = mode;
    return sock;
}

socket_t creerSocketAdr(int mode, char *adrIP, short port)
{
    struct sockaddr_in addr;
    adr2struct(&addr, adrIP, port);
    socket_t sock = creerSocket(mode);
    CHECK(bind(sock.fd, (struct sockaddr *) &addr, sizeof(addr)), "Erreur lors de l'adressage à la socket !");
    return sock;
}

socket_t creerSocketEcoute(char *adrIP, short port)
{
    socket_t sock = creerSocketAdr(SOCK_STREAM, adrIP, port);
    CHECK(listen(sock.fd, 5), "Erreur lors de l'écoute de la socket !");
    return sock;
}

socket_t accepterClt(const socket_t sockEcoute)
{
    struct sockaddr_in addr;
    socket_t sock;
    socklen_t len = sizeof(addr);
    CHECK(sock.fd = accept(sockEcoute.fd, (struct sockaddr *) &addr, &len), "Erreur lors de l'acceptation du client !");
    sock.addrLoc = sockEcoute.addrLoc;
    sock.mode = sockEcoute.mode;
    return sock;
}
/**
 *	\fn			socket_t connecterClt2Srv (char *adrIP, short port)
 *	\brief		Crétaion d'une socket d'appel et connexion au seveur dont
 *				l'adressage est fourni en paramètre
 *	\param		adrIP : adresse IP du serveur à connecter
 *	\param		port : port TCP du serveur à connecter
 *	\result		socket connectée au serveur fourni en paramètre
 */
socket_t connecterClt2Srv(char *adrIP, short port) {
    socket_t sock = creerSocket(SOCK_STREAM);
    adr2struct(&sock.addrDst, adrIP, port);
    CHECK(connect(sock.fd, (struct sockaddr *) &sock.addrDst, sizeof(sock.addrDst)), "Erreur lors de la connexion au serveur !");
    return sock;
}
