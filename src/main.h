/******************************************************************************/
/* MAIN.h                                                                     */
/******************************************************************************/
#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include "./partie.h" // on a besoin de connaître la structure Partie

typedef enum {
    SCENE_ACCUEIL = 0,
    SCENE_NIVEAU,
} SCENE;

void actualiser_jeu(Partie *p, Timer *t, SDL_Joystick *manette);
void dessiner_jeu(Partie *p);
void actualiser_accueil(Partie *p, Timer *t, SDL_Joystick *manette);
void activer_bouton(Partie *p, Timer *t, SDL_Joystick *manette);
void manger_bouton();
void dessiner_accueil();
int charger_niveau(Partie *p);
void charger_accueil();

#endif
