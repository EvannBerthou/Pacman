#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include "../lib/libgraphique.h"
#include "partie.h"

// Requête
int status(char *reponse);
void envoyer_score(Partie *p);
char *envoyer_requete(const char *host, int port, const char *req);

// Classement
void afficher_leaderboard();
void afficher_ligne(char *joueur, char *score, int y);
void afficher_message_leaderboard(char *message, int font);
char *entrer_nom();
void afficher_nom(char *nom, int index);

#endif
