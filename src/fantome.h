#ifndef FANTOME_H
#define FANTOME_H

#include "partie.h"
#include "path_finding.h"
#include "entite.h"

int charger_sprites_fantomes();
SDL_Surface *sprite_fantome(TypeEntite t, int dir, int frame);
void bouger_fantomes(Partie *p, float dt);
void dessiner_fantomes(Partie *p);
#endif
