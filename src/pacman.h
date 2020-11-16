#ifndef PACMAN_H
#define PACMAN_H

#include "partie.h"

int charger_sprites_pacman();
void bouger_pacman(Partie *p, float dt, int touche);
void dessiner_pacman(Partie *p);
SDL_Surface* sprite_pacman(int dir, int frame);

#endif
