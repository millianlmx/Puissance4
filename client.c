#include <pthread.h>
#include "logic.h"
#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <mcp23008.h>
#include <lcd.h>

#define BUTTONLEFT_PIN 6
#define BUTTONRIGHT_PIN 24
#define BUTTONBOTTOM_PIN 23
#define VIBRATOR_PIN 2

#define    MAX_LCDS    8

#define ADRESSELCD 0x21
#define AF_BASE 100
#define AF_STRB (AF_BASE+2)
#define AF_RW (AF_BASE+14)
#define AF_RS (AF_BASE+1)
#define AF_DB4 (AF_BASE+3)
#define AF_DB5 (AF_BASE+4)
#define AF_DB6 (AF_BASE+5)
#define AF_DB7 (AF_BASE+6)
#define AF_BLUE (AF_BASE+12)

#define AF_RED (107)

#define AF_BACKLIGHT_PIN (AF_BASE+15)
#define AF_BACKLIGHTGRD_PIN (AF_BASE+16)

#define SDA 3
#define SCL 5
#define ADRESSELCD 0x21
#define MAX_CAR 1000

char colors[12][10] = {RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, BOLD_RED, BOLD_GREEN, BOLD_YELLOW, BOLD_BLUE, BOLD_MAGENTA, BOLD_CYAN};

void reset(generic *quoi);
int askForColmun(plateau_t plateau, int line, int col, char colors[10]);
void senregistrer(char *serverIP, int serverPort, char *color, socket_t *sockEch);
int client(void *arg);

int initLcd(){
    int lcdHandle, i;
    int fd = wiringPiI2CSetup(ADRESSELCD);

    for(i=0;i<9;i++){
        wiringPiI2CWriteReg8(fd,i,0xFF);
    }
    wiringPiSetupSys();
    mcp23008Setup(AF_BASE,ADRESSELCD);
    pinMode(AF_BLUE,OUTPUT);
    lcdHandle = lcdInit(2, 17, 4, AF_RS, AF_STRB, AF_DB4,AF_DB5,AF_DB6,AF_DB7, 0,0,0,0);
    if(lcdHandle < 0){
        printf("lcdInit failed\n");
        return -1;
    }
    lcdDisplay(lcdHandle,1);
    return lcdHandle;
}

void effacerLigneLcd(int lcdHandle, int ligne) {
    lcdPosition(lcdHandle, 0, ligne); // Positionne le curseur au début de la ligne
    lcdPuts(lcdHandle, "                "); // Écrit des espaces pour effacer la ligne (ajustez le nombre d'espaces en fonction de la largeur de votre écran)
}

void ecrireLcd(int lcdHandle, int ligne, int colonne, char* msg){
    effacerLigneLcd(lcdHandle, ligne); // Efface la ligne spécifiée
    lcdPosition(lcdHandle, colonne, ligne); // Positionne le curseur
    lcdPuts(lcdHandle, msg); // Affiche le message
}

int main()
{
    CHECK(wiringPiSetup(), "Erreur lors de l'initialisation de WiringPi\n")
    int lcdHandle;
    lcdHandle = initLcd();
    CHECK(lcdHandle < 0 ? -1 : 0, "Erreur lors de l'initialisation du LCD\n")

    // Configuration des broches GPIO en entrée avec pull-up/down désactivé
    pinMode(BUTTONLEFT_PIN, INPUT);
    pinMode(BUTTONRIGHT_PIN, INPUT);
    pinMode(BUTTONBOTTOM_PIN, INPUT);
    pinMode(VIBRATOR_PIN, OUTPUT);

    buffer_t buffer;
    char *serverIP = malloc(17 * sizeof(char));
    char *serverPort = malloc(7 * sizeof(char));

    printf("Enter the game server IP : ");
    fgets(serverIP, 17, stdin);
    printf("Enter the game server port : ");
    fgets(serverPort, 7, stdin);

    serverIP[strlen(serverIP) - 1] = '\0';
    serverPort[strlen(serverPort) - 1] = '\0';

    char *color = malloc(12 * sizeof(char));
    socket_t *sockEch = malloc(sizeof(socket_t));

    senregistrer(serverIP, atoi(serverPort), color, sockEch);

    char *pseudo = malloc(200 * sizeof(char));
    printf("Enter your pseudo : ");
    fgets(pseudo, 200, stdin);
    pseudo[strlen(pseudo) - 1] = '\0';

    // Sending pseudo
    memset(buffer, 0, sizeof(buffer_t));
    strcpy(buffer, pseudo);
    envoyer(sockEch, pseudo, NULL);

    // Receiving the game infos
    memset(buffer, 0, sizeof(buffer_t));
    recevoir(sockEch, buffer, NULL);

    printf("Receiving game infos...\n");

    int line = atoi(strtok(buffer, "::"));
    int col = atoi(strtok(NULL, "::"));
    char *tmp = strtok(NULL, "::");
    tmp[1] = '\0';
    int nbPlayers = atoi(tmp);

    printf("Game infos received !\n");
    printf("Line : %d\n", line);
    printf("Column : %d\n", col);
    printf("Nb players : %d\n", nbPlayers);
    memset(buffer, 0, sizeof(buffer_t));

    plateau_t plateau = creerPlateau(line, col);
    printf("Plateau created !\n");

    int i = 0;
    maillon_t *playerList = NULL;

    // receiving the player data in the following format : id::colorIndex::pseudo
    memset(buffer, 0, sizeof(buffer_t));
    recevoir(sockEch, buffer, NULL);

    while (i < nbPlayers)
    {
        printf("Storing player %d infos...\n", i + 1);
        maillon_t *temp = malloc(sizeof(maillon_t));
        memset(temp, 0, sizeof(player_t));

        char *pseudo = malloc(200 * sizeof(char));
        char *color = malloc(11 * sizeof(char));
        memset(pseudo, 0, 200 * sizeof(char));
        memset(color, 0, 11 * sizeof(char));

        // Creation of the player and adding it to the player list
        if(i == 0) temp->id = atoi(strtok(&buffer[2], "::"));
        else temp->id = atoi(strtok(NULL, "::"));
        temp->colorIndex = atoi(strtok(NULL, "::"));
        strcpy(color, colors[temp->colorIndex]);
        temp->color = color;
        strcpy(pseudo, strtok(NULL, "::"));
        temp->pseudo = pseudo;
        temp->suivant = playerList;

        printf("Player details : %d::%d::%s\n", temp->id, temp->colorIndex, temp->pseudo);
        
        playerList = temp;

        i++;
    }

    afficherPlateau(plateau, line, col, colors);

    while (true)
    {
        memset(buffer, 0, sizeof(buffer_t));
        recevoir(sockEch, buffer, NULL);

        char *keyword = strtok(buffer, "::");
        if(strcmp(keyword, "YOURTURN") == 0)
        {
            int selectedCol = askForColmun(plateau, line, col, colors);
            memset(buffer, 0, sizeof(buffer_t));
            sprintf(buffer, "PLAYED::%d", selectedCol);
            envoyer(sockEch, buffer, NULL);
        }
        else if(strcmp(keyword, "PLAY") == 0)
        {
            int playerId = atoi(strtok(NULL, "::"));
            char *pseudo;
            maillon_t *temp = playerList;
            while(temp != NULL)
            {
                if(temp->id == playerId)
                {
                    pseudo = temp->pseudo;
                    break;
                }
                temp = temp->suivant;
            }
            int selectedCol = atoi(strtok(NULL, "::"));

            effacerLigneLcd(lcdHandle, 0);
            char msg[16];
            sprintf(msg, "%s joue !", pseudo);
            ecrireLcd(lcdHandle, 0, 0, msg);

            jouerJeton(plateau, line, selectedCol, playerId);
            effacerShell();
            afficherPlateau(plateau, line, col, colors);
        }
        else if(strcmp(keyword, "VICTORY") == 0)
        {
            int playerId = atoi(strtok(NULL, "::"));
            maillon_t *temp = playerList;
            while(temp != NULL)
            {
                if(temp->id == playerId)
                {
                    effacerLigneLcd(lcdHandle, 0);
                    char msg[16];
                    sprintf(msg, "%s a gagne !", temp->pseudo);
                    ecrireLcd(lcdHandle, 0, 0, msg);
                    break;
                }
                temp = temp->suivant;
            }
            break;
        }
    }
    return 0;
}

void senregistrer(char *serverIP, int serverPort, char *color, socket_t *sockEch)
{
    printf("Connecting to the server...\n");
    socket_t temp = connecterClt2Srv(serverIP, serverPort);

    memcpy(sockEch, &temp, sizeof(socket_t));
}

void reset(generic *quoi)
{
    memset(quoi, 0, sizeof(generic));
}

int askForColmun(plateau_t plateau, int line, int col, char colors[10])
{

    int column = 1;

    while (1) {
        effacerShell();
        // Lecture de l'état des boutons
        int buttonLeftState = digitalRead(BUTTONLEFT_PIN);
        int buttonRightState = digitalRead(BUTTONRIGHT_PIN);
        int buttonBottomState = digitalRead(BUTTONBOTTOM_PIN);

        if (!buttonBottomState) { // Le boutton bas est appuyé
            digitalWrite(VIBRATOR_PIN, 1);
            usleep(100000);
            digitalWrite(VIBRATOR_PIN, 0);
            break;
        }

	    if (!buttonLeftState && column > 1) {
            column -= 1; // Le boutton gauche est appuyé
            digitalWrite(VIBRATOR_PIN, 1);
            usleep(100000);
            digitalWrite(VIBRATOR_PIN, 0);
        }

	    if (!buttonRightState && column < col) {
            column += 1; // Le boutton droite est appuyé
            digitalWrite(VIBRATOR_PIN, 1);
            usleep(100000);
            digitalWrite(VIBRATOR_PIN, 0);
        }

        // Affichage du plateau
        afficherPlateau(plateau, line, col, colors);

        // Affichage de la colonne
	    printf("Colonne : %d (Appuyez sur le boutons bas pour confirmer)\n", column);

        // Attente de 100 ms avant la prochaine lecture
        delay(300);
    }

    return column - 1;
}