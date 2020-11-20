#include <math.h>
#include "pacman.h"
#include "partie.h"
#include "main.h"
#include "fantome.h"
#include "audio.h"

const char *pacman_sprite_path[4][2] = {
    {"data/sprites/pacman00.bmp", "data/sprites/pacman01.bmp"},
    {"data/sprites/pacman10.bmp", "data/sprites/pacman11.bmp"},
    {"data/sprites/pacman20.bmp", "data/sprites/pacman21.bmp"},
    {"data/sprites/pacman30.bmp", "data/sprites/pacman31.bmp"},
};

SDL_Surface *sprites_pacman[4][2];

// Charge les sprites de pacman
int charger_sprites_pacman() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 2; j++) {
            SDL_Surface *sprite = SDL_LoadBMP(pacman_sprite_path[i][j]);
            if (sprite == NULL) {
                fprintf(stderr, "Erreur lors du chargement du sprite %s\n", pacman_sprite_path[i][j]);
                return -1;
            }
            SDL_SetColorKey(sprite, SDL_SRCCOLORKEY, noir);
            sprites_pacman[i][j] = sprite;
        }
    }
    return 0;
}

static void reculer_pacman(Partie *p, Pos grille) {
    // temps que pacman est dans un mur faire demitour
    while (on_grid(p, grille.l, grille.c) == '*'){
        switch (p->pacman.etat.direction){
            case DIR_HAUT:   grille.l++; break;
            case DIR_BAS:    grille.l--; break;
            case DIR_GAUCHE: grille.c++; break;
            case DIR_DROITE: grille.c--; break;
        }
    }
    p->pacman.pos = (Posf){grille.l * CASE, grille.c * CASE};
}

// Vérifie si la touche pressé est l'opposée de la direction en cours
static int direction_opposee(Partie *p) {
    int curr = p->pacman.etat.direction;
    int next = p->pacman.etat.prochaine_direction;

    if (curr == DIR_HAUT   && next == DIR_BAS)    return 1;
    if (curr == DIR_BAS    && next == DIR_HAUT)   return 1;
    if (curr == DIR_GAUCHE && next == DIR_DROITE) return 1;
    if (curr == DIR_DROITE && next == DIR_GAUCHE) return 1;
    return 0;
}

// Vérifie si pacman est au centre de la case
static int centre_case(Partie *p) {
    float demi_x = CASE / 2;
    float demi_y = CASE / 2;
    return fmod(p->pacman.pos.l + demi_x, CASE) >= demi_x ||  fmod(p->pacman.pos.c + demi_y, CASE) >= demi_y;
}

// Renvoie la direction en fonction de la touche pressée
static int deplacement(int touche, int direction){
    switch (touche) {
        case SDLK_UP: return DIR_HAUT;
        case SDLK_DOWN: return DIR_BAS;
        case SDLK_LEFT: return DIR_GAUCHE;
        case SDLK_RIGHT: return DIR_DROITE;
        default: return direction;
    }
}

static int nouvelle_direction(Partie *p) {
    // Si pacman ne se déplace pas
    if (p->pacman.etat.direction == DIR_INCONNUE) return 1;
    return (
        // Si pacman est alligné à la grille ou part dans le direction opposé
        // Et qu'il ne se déplace pas dans un mur
        (aligne_grille(p, p->pacman.pos) || direction_opposee(p)) 
            && case_direction(p, &p->pacman, p->pacman.etat.prochaine_direction) != '*'
    );
}


//modification de la positon de pacman en fonction de l'environnement et des touches pressés
void bouger_pacman(Partie *p, float dt, int touche) {
    // Détermine la prochaine direction en fonction de la touche pressée
    p->pacman.etat.prochaine_direction = deplacement(touche, p->pacman.etat.prochaine_direction);

    // Si pacman ne se déplace dans aucune direciton
    // Ou qu'il a une direction en attente et qu'il atteint une intersection
    // Ou qu'il part dans dans la direction opposée
    // Alors il change de direction
    if (nouvelle_direction(p)) {
        p->pacman.etat.direction = p->pacman.etat.prochaine_direction;
        p->pacman.etat.prochaine_direction = DIR_INCONNUE;
    }

    // Si pacman est contre un mur alors ne pas bouger
    if (aligne_grille(p, p->pacman.pos) && case_direction(p, &p->pacman, p->pacman.etat.direction) == '*') {
        // Coupe le son si pacman est contre un mur 
        pause_son(p->son_pacman, 1);
        return;
    }

    switch (p->pacman.etat.direction){
        case DIR_HAUT:   p->pacman.pos.l -= dt * p->pacman.vitesse; break;
        case DIR_BAS:    p->pacman.pos.l += dt * p->pacman.vitesse; break;
        case DIR_GAUCHE: p->pacman.pos.c -= dt * p->pacman.vitesse; break;
        case DIR_DROITE: p->pacman.pos.c += dt * p->pacman.vitesse; break;
    }

    verifier_tunnel(p);

    // Animation_time étant un float, on vérifie manuellement si on doit boucler
    p->pacman.animation_time += dt * VITESSE_ANIMATION;
    if (p->pacman.animation_time >= 2) {
        p->pacman.animation_time = 0;
    }

    // Vérifie si pacman est en collision avec un bonbon
    pacman_manger(p);

    pause_son(p->son_pacman, 0);
}

void verifier_tunnel(Partie *p) {
    // Wrapping de pacman sur les bords
    if (p->pacman.pos.c > (p->C - 1) * CASE + CASE && p->pacman.etat.direction == DIR_DROITE) {
        p->pacman.pos.c = -(CASE * 2); // Téléporte à gauche
    }
    else if (p->pacman.pos.c < -CASE && p->pacman.etat.direction == DIR_GAUCHE) {
        p->pacman.pos.c = (p->C - 1) * CASE + CASE * 2; // Téléporte à droite
    }
    else if (p->pacman.pos.l > (p->L - 1) * CASE + CASE && p->pacman.etat.direction == DIR_BAS) {
        p->pacman.pos.l = -(CASE * 2); // Téléporte en haut
    }
    else if (p->pacman.pos.l < -CASE && p->pacman.etat.direction == DIR_HAUT) {
        p->pacman.pos.l = (p->L - 1) * CASE + CASE * 2; // Téléporte en bas
    }
}

void pacman_manger(Partie *p) {
    if (centre_case(p)) {
        Pos grille = ecran_vers_grille(p->pacman.pos);
        char type_case = on_grid(p, grille.l, grille.c);
        if (type_case == '.') { // Si sur une bille
            p->pacman.etat.score++;
            p->nbbonus--;
            #if DEBUG
            printf("bonbons : %d\n", p->nbbonus);
            #endif
        }
        else if (type_case == 'B') { // Si sur un bonbon
            p->pacman.etat.score += 50;
            for (int i = 0; i < p->nbf; i++){
                fuite_fantome(&p->fantomes[i]);
            }
            p->nbbonus--;
            activer_bonbon(p);
            #if DEBUG
            printf("bonbons : %d\n", p->nbbonus);
            #endif
        }
        if (type_case == '*') { // Si pacman est rentré dans un mur alors le faire reculer
            reculer_pacman(p, grille);
        }
        else if (type_case != 'x') { // Si pas en dehors du plateau
            p->plateau[grille.l][grille.c] = ' ';
        }
    }
}

SDL_Surface* sprite_pacman(int dir, int frame) {
    return sprites_pacman[dir][frame];
}

void dessiner_pacman(Partie *p) {
    Point pos = {p->pacman.pos.c, p->pacman.pos.l};
    // Pacman regarde vers le haut par défaut
    if (p->pacman.etat.direction == DIR_INCONNUE) {
        afficher_surface(sprite_pacman(1, 0), pos);
    }
    else {
        afficher_surface(sprite_pacman(p->pacman.etat.direction, (int)p->pacman.animation_time), pos);
    }
}

void pacman_mange_fantome(Partie *p, Entite *fantome) {
    p->fantomes_manges++;
    int score = pow(2, p->fantomes_manges) * 100;
    p->pacman.etat.score += score;

    // Affiche le score ajouté quand pacman mange un fantome
    char texte_score[5];
    sprintf(texte_score, "%d", score);
    Point pos = {fantome->pos.c, fantome->pos.l};
    afficher_texte(texte_score, 18, pos, rouge);
    //jouer son
    actualiser();
    attente(500);
}