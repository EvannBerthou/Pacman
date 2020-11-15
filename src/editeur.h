#ifndef EDITEUR_H
#define EDITEUR_H

#include "partie.h"

void lancer_editeur(char *chemin);
int sauvegarder_niveau(Partie *niveau);
Partie charger_editeur();

#endif