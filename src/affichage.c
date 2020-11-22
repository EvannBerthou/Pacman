#include <stdlib.h>
#include "main.h"
#include "affichage.h"
#include "fantome.h"
#include "pacman.h"
#include "entrer.h"

int _ecran_w = 600;
int _ecran_h = 540;

// Sprites de tous les élements en jeu
#define SPRITE_COUNT 19
const char *sprites_paths[SPRITE_COUNT] = {
    "data/sprites/accueil_logo.bmp",
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

// Charge les sprites au lancement du programme pour éviter de devoir les charger à chaque frame
int charger_sprites() {
    for (int i = 0; i < SPRITE_COUNT; i++) {
        SDL_Surface *sprite = SDL_LoadBMP(sprites_paths[i]);
        if (sprite == NULL) {
            fprintf(stderr, "Erreur lors du chargement du sprite %s\n", sprites_paths[i]);
            return -1;
        }
        SDL_SetColorKey(sprite, SDL_SRCCOLORKEY, noir);
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
short *voisins_murs = NULL;

// Renvoie le bon sprite
SDL_Surface *sprite_at(Partie *p, Point pos) {
    // Convertis les coordonnés 2d en index pour un array 1d
    int index = p->C * pos.y + pos.x;
    // Le 3 correspond à l'offset pour avoir les sprites des murs
    return sprites[3 + voisins_murs[index]];
}

SDL_Surface *sprite_index(int index) {
    return sprites[index];
}

void free_voisins() {
    free(voisins_murs);
    voisins_murs = NULL;
}

// Calcul le nombre de mur voisin pour chaque case
void calculer_voisins(Partie *p) {
    if (voisins_murs != NULL) {
        free_voisins();
    } 
    voisins_murs = malloc(sizeof(short) * p->L * p->C);
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
            voisins_murs[p->C * i + j] = voisins;
        }
    }
}

void afficher_bouton_selectionner() {
    int manette = manette_active(); // 1 si manette branchée sinon 0
    char *texte_boutons[] = {"Entrer : Selectionner", "X : Selectionner"};
    Point info_bouton = {0, ecran_h() - 26};
    afficher_texte(texte_boutons[manette] , 18, info_bouton, blanc);
}

void afficher_bouton_retour() {
    int manette = manette_active(); // 1 si manette branchée sinon 0
    char *texte_boutons[] = {"Q : Retour", "O : Retour"};
    Point info_bouton = {0, ecran_h() - 52};
    afficher_texte(texte_boutons[manette] , 18, info_bouton, blanc);
}

Point centrer_texte(char *texte, Point centre, int taille) {
    Point t = taille_texte(texte, taille);
    return (Point) {centre.x - (t.x / 2), centre.y - (t.y / 2)};
}

void afficher_secret_konami_code() {
    dessiner_rectangle((Point){0,0}, ecran_w(), ecran_h(), BG_COLOR);
    const int taille_titre = 36;
    char *titre = "Félicitations !";

    afficher_texte(titre, taille_titre,
                    centrer_texte(titre, (Point){ecran_w() / 2, 20}, taille_titre),
                    blanc);

    char *message[] = {
        "Félicitations, vous as pensés à faire le konami code",
        "(ou alors juste regardé le code directement)",
        "Comme récompenses, vous avez obtenu le droit de",
        "nous mettre une bonne note !",
    };

    int avance_y = 40;

    for (int i = 0; i < 4; i++) {
        afficher_texte(message[i], 18, (Point){0, avance_y}, blanc);
        avance_y += 20;
    }

    afficher_bouton_retour();
    actualiser();
    attendre_sortie();
}

void changer_taille_fenetre(int w, int h) {
    changer_resolution(w, h);
    _ecran_w = w;
    _ecran_h = h;
}

int ecran_w() {
    return _ecran_w;
}

int ecran_h() {
    return _ecran_h;
}