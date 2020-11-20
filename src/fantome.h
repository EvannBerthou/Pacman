#ifndef FANTOME_H
#define FANTOME_H

#include "partie.h"
#include "path_finding.h"
#include "entite.h"

#define TEMPS_FUITE 10.f
#define VITESSE_FANTOME 50

// Sprites
int charger_sprites_fantomes();
SDL_Surface *sprite_fantome(TypeEntite t, int dir, int frame);

// Déplacements
void reset_timer_fantomes();
void bouger_fantomes(Partie *p, float dt);
void determiner_cible(Partie *p, Entite *fantome);
DirEntite determiner_direction(Entite *fantome);

// IA
void IA_ORANGE(Partie *p, Entite *fantome);
void select_coin(Entite *fantome);

void IA_PINK(Partie *p, Entite *fantome);
void IA_CYAN(Entite *fantome);

// Affichage
void dessiner_fantomes(Partie *p);

// Outils
void fuite_fantome(Entite *fantome);
void a_ete_mange(Entite *fantome);
void revivre(Entite *fantome);
float distance_pac(Posf A,Posf B);
void calculer_vitesse_niveau(Partie *p);


#endif
