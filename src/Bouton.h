#ifndef BOUTON_H
#define BOUTON_H

#include "../lib/libgraphique.h"

typedef struct {
    SDL_Rect rect;
    int taille;
    Couleur c;
    char *texte;
} BoutonAccueil;

BoutonAccueil nouveau_bouton(Point centre, Couleur c, char *texte, int taille);

#endif