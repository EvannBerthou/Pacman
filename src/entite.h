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
    DIR_HAUT,    // 1
    DIR_BAS,     // 2
    DIR_GAUCHE,  // 3
    DIR_DROITE   // 4
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
} Entite;

Entite nouvelle_entite(Pos pos,Pos pos_init ,TypeEntite type);
Pos ecran_vers_grille(Pos pos, Pos taille);

#endif
