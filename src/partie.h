/******************************************************************************/
/* CHARGEMENT.h                                                              */
/******************************************************************************/
#ifndef CHARGEMENT_H
#define CHARGEMENT_H

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include "../lib/libgraphique.h"
#include "timer.h"
#include "entite.h"

#define NBFANTOMES  4 // nombres de fantômes dans les plateaux chargés

// STRUCTURES
/* Structure Partie:  permet de stocker les paramètres d'une partie           */
typedef struct partie {
    char ** plateau; // le tableau de caractères contenant le plateau de jeu
    int     L; // le nb de lignes du plateau de jeu
    int     C; // le nb de colonne du plateau de jeu
    Entite  pacman; // l'entité pacman
    Entite  fantomes[NBFANTOMES]; // Liste des entités fantomes
    int     nbbonus; // le nombre de bonus restants à manger
    // ...et vous pouvez ajouter tout ce dont vous avez besoin
} Partie;

// PROTOTYPES
/* charge_plan : lit un fichier contenant un plateau de jeu et le charge
                    en mémoire, dans le champ 'plateau' d'une Partie */
Partie charge_plan(char *fichier);
int charger_sprites();
char on_grid(Partie *p, int l, int c);
int aligne_grille(Partie *p, Posf pos);
void calculer_voisins(Partie *p);
char case_direction(Partie *p, Entite *e, int direction);
void actualiser_partie(Partie *p, Timer *timer, SDL_Joystick *manette);
void dessiner_grille(Partie *p, int dans_editeur);
void dessiner_texte(Partie *p);
void dessiner_partie(Partie *p);
void terminer_partie(Partie *p);
char *entrer_nom();
void afficher_nom(char *nom, int index);

#endif
