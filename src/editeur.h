#ifndef EDITEUR_H
#define EDITEUR_H

#include "partie.h"

void lancer_editeur(char *chemin);
Partie charger_plan_editeur(char *chemin);
Partie charger_editeur();
int sauvegarder_niveau(Partie *niveau);

#endif