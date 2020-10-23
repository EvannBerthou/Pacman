/******************************************************************************/
/* CHARGEMENT.h                                                              */
/******************************************************************************/
#ifndef CHARGEMENT_H
#define CHARGEMENT_H

#include <stdio.h>
#include <stdlib.h>
#include "../lib/libgraphique.h"
#include "timer.h"
#include "entite.h"

#define NBFANTOMES  1 // nombres de fantômes dans les plateaux chargés
#define PACMAN_DELAY 125

// STRUCTURES
/* Structure Partie:  permet de stocker les paramètres d'une partie           */
typedef struct partie {
    char ** plateau; // le tableau de caractères contenant le plateau de jeu
    int     L; // le nb de lignes du plateau de jeu
    int     C; // le nb de colonne du plateau de jeu
    Entite  pacman; // l'entité pacman
    Entite     fantomes[NBFANTOMES]; // les positions de chaque fantôme
    int     nbbonus; // le nombre de bonus restants à manger
    // ...et vous pouvez ajouter tout ce dont vous avez besoin
    int taille_case[2];
} Partie;

// PROTOTYPES
/* charge_plan : lit un fichier contenant un plateau de jeu et le charge
                    en mémoire, dans le champ 'plateau' d'une Partie */
Partie charge_plan(char *fichier);
void actualiser_partie(Partie *p, Timer *timer);
void dessiner_partie(Partie *p);

#endif
