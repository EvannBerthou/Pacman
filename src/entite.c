#include "entite.h"

Entite nouvelle_entite(Pos pos,Pos pos_init, TypeEntite type) {
    Etat etat = {
        .direction = DIR_INCONNUE,
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
    };
    for (int i = 0; i < 4; i++) {
        result.sprite[i] = charger_sprite(type, i);
    }
    return result;
}

// Dans le meme ordre que l'enum TypeEntite
const char *entites_sprites_path[][4] = {
    {
        "data/sprites/pacman0.bmp",
        "data/sprites/pacman1.bmp",
        "data/sprites/pacman2.bmp",
        "data/sprites/pacman3.bmp",
    },
    {
        "data/sprites/fantome0.bmp",
        "data/sprites/fantome1.bmp",
        "data/sprites/fantome2.bmp",
        "data/sprites/fantome3.bmp",
    }
};

SDL_Surface *charger_sprite(TypeEntite type, int dir) {
    SDL_Surface *img = SDL_LoadBMP(entites_sprites_path[type][dir]);
    if (img == NULL) {
        fprintf(stderr, "Erreur lors du chargement du sprite %s\n", entites_sprites_path[type][dir]);
        exit(1);
    }
    // Transparence noire
    //SDL_SetColorKey(img, SDL_SRCCOLORKEY, 0xFFFFFF);
    return img;
}


Pos ecran_vers_grille(Pos pos, Pos taille) {
    return (Pos) {
        pos.l  / taille.l,
        pos.c  / taille.c,
    };
}