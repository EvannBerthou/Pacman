#ifndef ENTITE_H
#define ENTITE_H

#include <SDL/SDL.h>

/* Structure Pos: permet de stocker un couple ligne/colonne                   */
typedef struct pos {
    int     l;
    int     c;
} Pos;

typedef struct posf {
    float l,c;
} Posf;

typedef enum {
    ENTITE_FANTOME_R,// fantome red
    ENTITE_FANTOME_P,// fantome pink
    ENTITE_FANTOME_C,// fantome cyan
    ENTITE_FANTOME_O,// fantome orange
    ENTITE_PACMAN,
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
    int direction; // Direction de l'entité
    int prochaine_direction; // Direction de l'entité à la prochaine intersection
    int score; // score de pacman
    float fuite; // temps restant de fuite
    int nb_vie; // nombre de vie de pacman
    int mange; // si le fantome s'est fait manger
    int suiv; //suivre ke cchemin
} Etat;



typedef struct Noeud{
    Pos pos;
    int cout_g, cout_h, cout_t ;
    struct Noeud * parent;
} Noeud;



typedef struct {
    Posf pos;
    Posf pos_init;
    Posf pos_cible;
    TypeEntite type;
    Etat etat;
    Noeud* chemin_noeud[500];
    int nombre_noeud;
    int nombre_frames;
    float vitesse;
    float animation_time;
} Entite;


Entite nouvelle_entite(Posf pos, Posf pos_init, TypeEntite type);
SDL_Surface *charger_sprite(TypeEntite type, int dir, int frame);
Pos ecran_vers_grille(Posf pos);

#endif
