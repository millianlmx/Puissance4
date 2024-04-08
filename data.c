/**
 *	\file		data.h
 *	\brief		Spécification de la couche Data Representation
 *	\author		Samir El Khattabi
 *	\date		3 mars 2023
 *	\version	1.0
 */

#include "data.h"
#include <stdarg.h>

void envoyer(socket_t *sockEch, generic quoi, pFct serial, ...) {
    va_list args;
    va_start(args, serial);

    if (serial != NULL) {
        // Serialize the data
        serial(quoi, args);
    } else {
        // Treat quoi as a string
        const char* str = (const char*)quoi;
        // Send the string over the socket
        CHECK(send(sockEch->fd, str, strlen(str), 0), "Can't send the string !")
    }

    va_end(args);
}

void recevoir(socket_t *sockEch, generic quoi, pFct deSerial) {
    if (sockEch->mode == SOCK_DGRAM) {
        // Receive the data
        CHECK(recv(sockEch->fd, quoi, sizeof(quoi), 0), "Can't receive the data !")
    } else {
        // Receive the data
        char buffer[1024];
        CHECK(recv(sockEch->fd, buffer, sizeof(buffer), 0), "Can't receive the data !")
        // Deserialize the data
        deSerial(buffer, quoi);
    }
}
