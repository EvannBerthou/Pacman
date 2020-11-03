#ifndef ENTITE_H
#define ENTITE_H

#include <SDL/SDL.h>

/* Structure Pos: permet de stocker un couple ligne/colonne                   */
typedef struct pos {
    int     l;
    int     c;
} Pos;

typedef struct posf {
    float x,y;
} Posf;

typedef enum {
    ENTITE_PACMAN = 0,
    ENTITE_FANTOME,
} TypeEntite;

typedef enum {
    DIR_HAUT,     // 1 = 0b0001
    DIR_BAS,      // 2 = 0b0010
    DIR_GAUCHE,   // 3 = 0b0100
    DIR_DROITE,   // 4 = 0b1000
    DIR_INCONNUE
} DirEntite;

/* Structure element permet de stocker la direction de deplacement, le score de pacman et mode chase des fantome */
typedef struct {
    int direction;
    int prochaine_direction;
    int score;
    int fuite;
    int nb_vie;
} Etat;

typedef struct {
    Pos pos;
    Pos pos_init;
    TypeEntite type;
    Etat etat;
    // Liste des sprites de l'entité
    // 4 sprite pour chacune des 4 directions
    // L'ordre correspond à l'odre de DirEntite
    SDL_Surface *sprite[4];
} Entite;

Entite nouvelle_entite(Pos pos,Pos pos_init ,TypeEntite type);
SDL_Surface *charger_sprite(TypeEntite type, int dir);
Pos ecran_vers_grille(Pos pos, Pos taille);

#endif
