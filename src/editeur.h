#ifndef EDITEUR_H
#define EDITEUR_H

#include "partie.h"

void lancer_editeur();
int sauvegarder_niveau(Partie *niveau);
Partie charger_editeur(char *chemin);

#endif