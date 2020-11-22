#include "main.h"
#include "affichage.h"
#include "fantome.h"
#include "pacman.h"
#include "entrer.h"

#define SPRITE_COUNT 18
const char *sprites_paths[SPRITE_COUNT] = {
    "data/sprites/bille.bmp",
    "data/sprites/bonbon.bmp",
    "data/sprites/mur0.bmp",
    "data/sprites/mur1.bmp",
    "data/sprites/mur2.bmp",
    "data/sprites/mur3.bmp",
    "data/sprites/mur4.bmp",
    "data/sprites/mur5.bmp",
    "data/sprites/mur6.bmp",
    "data/sprites/mur7.bmp",
    "data/sprites/mur8.bmp",
    "data/sprites/mur9.bmp",
    "data/sprites/mur10.bmp",
    "data/sprites/mur11.bmp",
    "data/sprites/mur12.bmp",
    "data/sprites/mur13.bmp",
    "data/sprites/mur14.bmp",
    "data/sprites/mur15.bmp",
};
SDL_Surface* sprites[SPRITE_COUNT];

int charger_sprites() {
    for (int i = 0; i < SPRITE_COUNT; i++) {
        SDL_Surface *sprite = SDL_LoadBMP(sprites_paths[i]);
        if (sprite == NULL) {
            fprintf(stderr, "Erreur lors du chargement du sprite %s\n", sprites_paths[i]);
            return -1;
        }
        sprites[i] = sprite;
    }
    if (charger_sprites_fantomes() == -1) {
        return -1;
    }
    if (charger_sprites_pacman() == -1) {
        return -1;
    }
    return 0;
}


// Calculer un nombre pour déterminer le nombre de voisins puis le stocker dans le meme ordre
// que le plateau
short voisins_murs[27 * 21] = {};

// Renvoie le bon sprite
SDL_Surface *sprite_at(Point pos) {
    // Convertis les coordonnés 2d en index pour un array 1d
    int index = 21 * pos.y + pos.x;
    // Le 2 correspond à l'offset pour avoir les sprites des murs
    return sprites[2 + voisins_murs[index]];
}

SDL_Surface *sprite_index(int index) {
    return sprites[index];
}

void calculer_voisins(Partie *p) {
    for (int i = 0; i < p->L; i++) {
        for (int j = 0; j < p->C; j++) {
            short voisins = 0;
            /*
             Ici on encode les voisins mais au lieu de stocker si un mur a des voisins
             dans 4 variables pour chaque direction, on stocke tous les voisins dans une seule variable
             Cela permet d'économiser beaucoup de mémoire
             Un int est encodé sur 4 bytes (du moins sur ma machine), ce qui donne :
             27 * 21 * 4 (int) * 4 (voisins) = 9,072 bytes
             En utilisant d'un seul short de 2 bytes, ça donne
             27 * 21 * 2 (short) * 1 (une seule variable) = 1,134 bytes
             Cela permet en plus de plus facilement déterminer quel sprite doit être afficher
             pour chaque mur
            */
            if (on_grid(p, i - 1, j) == '*')
                voisins |= 0b0001;
            if (on_grid(p, i + 1, j) == '*')
                voisins |= 0b0010;
            if (on_grid(p, i, j - 1) == '*')
                voisins |= 0b0100;
            if (on_grid(p, i, j + 1) == '*')
                voisins |= 0b1000;
            voisins_murs[21 * i + j] = voisins;
        }
    }
}

void afficher_bouton_selectionner() {
    int manette = manette_active(); // 1 si manette branchée sinon 0
    char *texte_boutons[] = {"Entrer : Selectionner", "X : Selectionner"};
    Point info_bouton = {0, ECRAN_H - 26};
    afficher_texte(texte_boutons[manette] , 18, info_bouton, blanc);
}

void afficher_bouton_retour() {
    int manette = manette_active(); // 1 si manette branchée sinon 0
    char *texte_boutons[] = {"Q : Retour", "O : Retour"};
    Point info_bouton = {0, ECRAN_H - 52};
    afficher_texte(texte_boutons[manette] , 18, info_bouton, blanc);
}

Point centrer_texte(char *texte, Point centre, int taille) {
    Point t = taille_texte(texte, taille);
    return (Point) {centre.x - (t.x / 2), centre.y - (t.y / 2)};
}