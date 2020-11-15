#include <math.h>
#include "entite.h"
#include "main.h"

Entite nouvelle_entite(Posf pos, Posf pos_init, TypeEntite type) {
    Etat etat = {
        .direction = DIR_INCONNUE,
        .prochaine_direction = DIR_INCONNUE,
        .score = 0,
        .fuite = 0,
        .tmp_fuite=0,
        .nb_vie =2,
        .manger=0,
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

    if (type == ENTITE_FANTOME_R ){
        result.nombre_frames = 1;
        result.vitesse=50;
    }// Pacman (pas ENTITE_FANTOME) possède 2 frames d'animation : bouche ouverte/fermée
    else {
        result.nombre_frames = 2;
        result.vitesse=75;
    }
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
        {"data/sprites/fantomeO00.bmp"}, 
        {"data/sprites/fantomeO10.bmp"}, 
        {"data/sprites/fantomeO20.bmp"},
        {"data/sprites/fantomeO30.bmp"},
    }
};
/*   nom des sprite fantome ajouter 
    {
        sprite de fantomes en mode normale O pour fantome orange 

        {"data/sprites/fantomeO00.bmp","data/sprites/fantomeO01.bmp"},  
        {"data/sprites/fantomeO10.bmp","data/sprites/fantomeO11.bmp"}, 
        {"data/sprites/fantomeO20.bmp","data/sprites/fantomeO21.bmp"},
        {"data/sprites/fantomeO30.bmp","data/sprites/fantomeO31.bmp"},
    },
    {
        sprite de fantomes en mode normale P pour fantome pink 

        {"data/sprites/fantomeP00.bmp","data/sprites/fantomeP01.bmp"},  
        {"data/sprites/fantomeP10.bmp","data/sprites/fantomeP11.bmp"}, 
        {"data/sprites/fantomeP20.bmp","data/sprites/fantomeP21.bmp"},
        {"data/sprites/fantomeP30.bmp","data/sprites/fantomeP31.bmp"},
    },
    {
        sprite de fantomes en mode normale R pour fantome red 

        {"data/sprites/fantomeR00.bmp","data/sprites/fantomeR01.bmp"},  
        {"data/sprites/fantomeR10.bmp","data/sprites/fantomeR11.bmp"}, 
        {"data/sprites/fantomeR20.bmp","data/sprites/fantomeR21.bmp"},
        {"data/sprites/fantomeR30.bmp","data/sprites/fantomeR31.bmp"},
    }
    {
        sprite de fantomes en mode normale B pour fantome orange 

        {"data/sprites/fantomeB00.bmp","data/sprites/fantomeB01.bmp"},  
        {"data/sprites/fantomeB10.bmp","data/sprites/fantomeB11.bmp"}, 
        {"data/sprites/fantomeB20.bmp","data/sprites/fantomeB21.bmp"},
        {"data/sprites/fantomeB30.bmp","data/sprites/fantomeB31.bmp"},
    }




    {
        sprites fantome mode fuite avec tmp_fuite <5000ms
        {"data/sprites/fantome_fuite_bleu0.bmp","data/sprites/fantome_fuite_bleu1.bmp"}, 
    }
    {
        sprite fantome mode fuite avec tmp > 5000 ms 
        alerne entre celui la et le precedant a une frequence de plus en plus rapite jusqu'a tmp_fuite >=10000
        {"data/sprites/fantome_fuite_blanc0.bmp","data/sprites/fantome_fuite_blanc1.bmp"}, 

    }

    {
        sprite fantome mode manger 
        {"data/sprites/fantome_manger0.bmp"}, 
        {"data/sprites/fantome_manger1.bmp"}, 
        {"data/sprites/fantome_manger2.bmp"},
        {"data/sprites/fantome_manger3.bmp"},
    }


}
*/

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