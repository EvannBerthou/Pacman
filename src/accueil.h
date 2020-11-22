#ifndef ACCUEIL_H
#define ACCUEIL_H

#include "partie.h"
#include "timer.h"

void charger_boutons();
void actualiser_accueil(Partie *p, Timer *t);
void activer_bouton(Partie *p, Timer *t);
void manger_bouton();
char* selectionner_niveau();
void charger_accueil();

// Affichage
void dessiner_accueil();
void afficher_instructions();
void afficher_liste_niveaux(char **liste, int n, int curseur);

#endif
