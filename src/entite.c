#include <math.h>
#include "entite.h"
#include "main.h"

Entite nouvelle_entite(Posf pos, Posf pos_init, TypeEntite type) {
    Etat etat = {
        .direction = DIR_INCONNUE,
        .prochaine_direction = DIR_INCONNUE,
        .score = 0,
        .fuite = 0,
        .nb_vie =2,
        .suiv=0,
    };
    Entite result = {
        .pos = pos,
        .pos_init = pos_init,
        .pos_cible=(Posf){0,0},
        .type = type,
        .etat = etat,
        .animation_time = 0,
        .nombre_noeud=0,
        .chemin_noeud={0},
    };

    if (type == ENTITE_FANTOME_R )
        result.nombre_frames = 1;
    // Pacman (pas ENTITE_FANTOME) possède 2 frames d'animation : bouche ouverte/fermée
    else 
        result.nombre_frames = 2;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < result.nombre_frames; j++) {
            result.sprite[i][j] = charger_sprite(type, i, j);
        }
    }
    return result;
}

// Premier tableau correspond au type de l'entité, 
// Deuxiemes aux sprites des directions,
// troisiemes aux frames de l'animation (seul pacman a une animation mais cela rend le jeu plus extensible facilement)
const char *entites_sprites_path[][4][2] = {
    {
        {"data/sprites/pacman00.bmp", "data/sprites/pacman01.bmp"},
        {"data/sprites/pacman10.bmp", "data/sprites/pacman11.bmp"},
        {"data/sprites/pacman20.bmp", "data/sprites/pacman21.bmp"},
        {"data/sprites/pacman30.bmp", "data/sprites/pacman31.bmp"},
    },
    {
        {"data/sprites/fantome0.bmp"}, 
        {"data/sprites/fantome1.bmp"}, 
        {"data/sprites/fantome2.bmp"},
        {"data/sprites/fantome3.bmp"},
    }
};

SDL_Surface *charger_sprite(TypeEntite type, int dir, int frame) {
    SDL_Surface *img = SDL_LoadBMP(entites_sprites_path[type][dir][frame]);
    if (img == NULL) {
        fprintf(stderr, "Erreur lors du chargement du sprite %s\n", entites_sprites_path[type][dir][frame]);
        exit(1);
    }
    // Transparence noire
    SDL_SetColorKey(img, SDL_SRCCOLORKEY, 0x000000);
    return img;
}

Pos ecran_vers_grille(Posf pos) {
    // Convertis taille en Posf pour ne pas perdre la précision lors de la division
    Posf taille_f = { CASE, CASE }; 
    return (Pos) {
        (int)(roundf (pos.l  / taille_f.l)),
        (int)(roundf (pos.c  / taille_f.c))
    };
}