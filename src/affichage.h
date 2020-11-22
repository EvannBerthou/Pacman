#ifndef AFFICHAGE_H
#define AFFICHAGE_H

#include "../lib/libgraphique.h"
#include "partie.h"

// Sprite
int charger_sprites();
SDL_Surface *sprite_at(Partie *p, Point pos);
SDL_Surface *sprite_index(int index);
void free_voisins();

// Affichage
void afficher_bouton_selectionner();
void afficher_bouton_retour();
Point centrer_texte(char *texte, Point centre, int taille);
void afficher_secret_konami_code();
void changer_taille_fenetre(int w, int h);

int ecran_w();
int ecran_h();

#endif