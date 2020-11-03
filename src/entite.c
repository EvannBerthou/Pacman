#include "entite.h"

Entite nouvelle_entite(Pos pos,Pos pos_init, TypeEntite type) {
    Etat etat = {
        .direction = 0,
        .prochaine_direction = 0,
        .score = 0,
        .fuite = 0,
        .nb_vie = 3,
    };
    Entite result = {
        .pos = pos,
        .pos_init = pos_init,
        .type = type,
        .etat = etat,
        .sprite = charger_sprite(type)
    };
    return result;
}

// Dans le meme ordre que l'enum TypeEntite
const char *sprites_path[] = {
    "data/sprites/pacman.bmp",
    "data/sprites/fantome.bmp"
};

SDL_Surface *charger_sprite(TypeEntite type) {
    SDL_Surface *img = SDL_LoadBMP(sprites_path[type]);
    if (img == NULL) {
        fprintf(stderr, "Erreur lors du chargement du sprite %s\n", sprites_path[type]);
        exit(1);
    }
    SDL_SetColorKey(img, SDL_SRCCOLORKEY, 0xFFFFFF);
    return img;
}


Pos ecran_vers_grille(Pos pos, Pos taille) {
    return (Pos) {
        pos.l  / taille.l,
        pos.c  / taille.c,
    };
}