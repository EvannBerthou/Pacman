/******************************************************************************/
/* MAIN.h                                                                     */
/******************************************************************************/
#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include "./partie.h" // on a besoin de connaître la structure Partie

#define CASE 20 
#define VITESSE_ANIMATION 7
#define BG_COLOR 0x000595

typedef enum {
    SCENE_ACCUEIL = 0,
    SCENE_NIVEAU,
} SCENE;

void actualiser_jeu(Partie *p, Timer *t);
void dessiner_jeu(Partie *p);
void changer_scene(SCENE s);

#endif
