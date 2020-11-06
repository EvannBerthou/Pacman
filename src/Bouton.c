#include "Bouton.h"

BoutonAccueil nouveau_bouton(Point centre, Couleur c, char *texte, int taille) {
    BoutonAccueil resultat;
    resultat.taille = taille;
    resultat.c = c;
    resultat.texte = texte;
    Point tt = taille_texte(texte, taille);

    resultat.rect = (SDL_Rect){
        centre.x - (tt.x / 2),
        centre.y,
        tt.x, tt.y
    };

    return resultat;
}