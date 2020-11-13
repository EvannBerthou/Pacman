#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include "../lib/libgraphique.h"

int status(char *reponse);
char *envoyer_requete(const char *host, int port, const char *req);
void afficher_leaderboard(SDL_Joystick *manette);
void afficher_ligne(char *joueur, char *score, int y);
void afficher_message_leaderboard(char *message, int font);

#endif
