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


void actualiser_jeu(Partie *p, Timer *t);
void dessiner_jeu(Partie *p);
void actualiser_accueil();
int charger_niveau(Partie *p);

#endif
