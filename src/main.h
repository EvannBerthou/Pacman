/******************************************************************************/
/* MAIN.h                                                                     */
/******************************************************************************/
#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include "./partie.h" // on a besoin de connaître la structure Partie

#define ECRAN_W 600
#define PLATEAU_W 420
#define ECRAN_H 540
#define CASE 20
#define VITESSE_ANIMATION 7

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
char* selectionner_niveau(SDL_Joystick *manette);
int charger_niveau(Partie *p, char *chemin);
void charger_accueil();
void afficher_liste_niveaux(char **liste, int n, int curseur);
int nouvelle_touche(SDL_Joystick *manette);

#endif
