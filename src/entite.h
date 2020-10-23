#ifndef ENTITE_H
#define ENTITE_H

/* Structure Pos: permet de stocker un couple ligne/colonne                   */
typedef struct pos {
    int     l;
    int     c;
} Pos;

typedef enum {
    ENTITE_PACMAN = 0,
    ENTITE_FANTOME
} TypeEntite;

/* Structure element permet de stocker le sens de deplacement le score de pacman et mode chase des fantome */
typedef struct {
    int sens;
    int prochain_sens;
    int score;
    int fuite;
    int nb_vie;
} Etat;

typedef struct {
    Pos pos;
    TypeEntite type;
    Etat etat;
    int delai_deplacement; // le temps restant avant que l'entité se déplace
} Entite;

Entite nouvelle_entite(Pos pos, TypeEntite type);

#endif
