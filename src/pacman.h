#ifndef PACMAN_H
#define PACMAN_H

#include "partie.h"

// Déplacement
void bouger_pacman(Partie *p, float dt, int touche);
void pacman_manger(Partie *p);
void verifier_tunnel(Partie *p);

// Affichage
int charger_sprites_pacman();
void dessiner_pacman(Partie *p);
SDL_Surface* sprite_pacman(int dir, int frame);

#endif
