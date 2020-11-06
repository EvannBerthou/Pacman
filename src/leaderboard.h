#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include "../lib/libgraphique.h"

char *envoyer_requete(const char *host, int port, const char *req);
void afficher_leaderboard();
void afficher_ligne(char *joueur, char *score, int y);

#endif