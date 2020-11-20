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

// Combien de temps le bonbon reste actif
#define TEMPS_FUITE 10.f 

// STRUCTURES
/* Structure Partie:  permet de stocker les paramètres d'une partie           */
typedef struct partie {
    char ** plateau; // le tableau de caractères contenant le plateau de jeu
    int     L; // le nb de lignes du plateau de jeu
    int     C; // le nb de colonne du plateau de jeu
    int pacman_place; // Pacman est-il sur le plateau ? (utilé pour l'éditeur)
    Entite  pacman; // l'entité pacman
    int son_pacman; // Son de déplacement de pacman
    int nbf; // Nombre de fantomes
    Entite  fantomes[4]; // Liste des entités fantomes
    int     nbbonus; // le nombre de bonus restants à manger
    int niveau; // Combien de fois le niveau a été relancé cette partie
    char *nom_fichier; // Nom du fichier correspondant à la partie
    float temps_bonbon; // Temps actif restant du bonbon
    int fantomes_manges; // Nombre de fantomes qui ont étés mangés avec le bonbon actif
} Partie;

// Chargements
/* charge_plan : lit un fichier contenant un plateau de jeu et le charge
                    en mémoire, dans le champ 'plateau' d'une Partie */
int charger_plan(char *chemin, Partie *p);
int charger_niveau(Partie *p, char *chemin);
int charger_sprites();
void calculer_voisins(Partie *p);

// Actualisation
void actualiser_partie(Partie *p, Timer *timer);

// Affichage
void dessiner_grille(Partie *p, int dans_editeur);
void dessiner_texte(Partie *p);
void dessiner_partie(Partie *p);

// Outils
int aligne_grille(Partie *p, Posf pos);
char case_direction(Partie *p, Entite *e, int direction);
char on_grid(Partie *p, int l, int c);
void vider_partie(Partie *p);
void terminer_partie(Partie *p);
void relancer_niveau(Partie *p);
void activer_bonbon(Partie *p);
void verifier_temps_bonbon_restant(Partie *p, float dt);

#endif
