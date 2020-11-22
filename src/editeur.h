#ifndef EDITEUR_H
#define EDITEUR_H

#include "partie.h"

void lancer_editeur(char *chemin);
Partie charger_editeur(char *chemin);
int sauvegarder_niveau(Partie *niveau);

#endif