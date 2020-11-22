#ifndef BOUTON_H
#define BOUTON_H

#include "../lib/libgraphique.h"

typedef struct {
    SDL_Rect rect; // Positon et taille du bouton
    int taille; // Taille de la police du bouton
    Couleur c;  // Couleur du texte
    char *texte; // Texte du bouton
} BoutonAccueil;

BoutonAccueil nouveau_bouton(Point centre, Couleur c, char *texte, int taille);

#endif