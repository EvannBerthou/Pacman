#ifndef ENTITE_H
#define ENTITE_H

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
    DIR_INCONNUE = 0,
    DIR_HAUT,
    DIR_BAS,
    DIR_GAUCHE,
    DIR_DROITE
} DirEntite;

/* Structure element permet de stocker la direction de deplacement, le score de pacman et mode chase des fantome */
typedef struct {
    int direction;
    int prochain_sens;
    int score;
    int fuite;
    int nb_vie;
} Etat;

typedef struct {
    Pos pos;
    TypeEntite type;
    Etat etat;
} Entite;

Entite nouvelle_entite(Pos pos, TypeEntite type);
Pos ecran_vers_grille(Pos pos, Pos taille);

#endif
