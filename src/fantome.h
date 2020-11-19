#ifndef FANTOME_H
#define FANTOME_H

#include "partie.h"
#include "path_finding.h"
#include "entite.h"

#define TEMPS_FUITE 10.f

int charger_sprites_fantomes();
SDL_Surface *sprite_fantome(TypeEntite t, int dir, int frame);
void reset_timer_fantomes();
void bouger_fantomes(Partie *p, float dt);
void dessiner_fantomes(Partie *p);
void select_coin(Entite *fantome);
void fuite_fantome(Entite *fantome);
void a_ete_mange(Entite *fantome);
void revivre(Entite *fantome);
float distance_pac(Posf A,Posf B);

#endif
