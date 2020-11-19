#ifndef ACCUEIL_H
#define ACCUEIL_H

#include "partie.h"
#include "timer.h"

void charger_boutons();
void actualiser_accueil(Partie *p, Timer *t);
void activer_bouton(Partie *p, Timer *t);
void dessiner_accueil();
void manger_bouton();
char* selectionner_niveau();
void afficher_liste_niveaux(char **liste, int n, int curseur);
void charger_accueil();

#endif
