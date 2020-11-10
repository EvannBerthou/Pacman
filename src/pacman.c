#include <math.h>
#include "pacman.h"
#include "partie.h"
#include "main.h"

#define VITESSE_ANIMATION 7

static int direction_opposee(Partie *p) {
    int curr = p->pacman.etat.direction;
    int next = p->pacman.etat.prochaine_direction;

    if (curr == DIR_HAUT   && next == DIR_BAS)    return 1;
    if (curr == DIR_BAS    && next == DIR_HAUT)   return 1;
    if (curr == DIR_GAUCHE && next == DIR_DROITE) return 1;
    if (curr == DIR_DROITE && next == DIR_GAUCHE) return 1;
    return 0;
}

static int centre_case(Partie *p) {
    float demi_x = CASE / 2;
    float demi_y = CASE / 2;
    return fmod(p->pacman.pos.l + demi_x, CASE) >= demi_x ||  fmod(p->pacman.pos.c + demi_y, CASE) >= demi_y;
}

//modification de la positon de pacman en fonction de l'environnement et des touches pressés
void bouger_pacman(Partie *p, float dt) {
    // Si pacman ne se déplace dans aucune direciton
    // Ou qu'il a une direction en attente et qu'il atteint une intersection
    // Ou qu'il part dans dans la direction opposée
    // Alors il change de direction
    if (p->pacman.etat.direction == DIR_INCONNUE
        || ((aligne_grille(p, p->pacman.pos) || direction_opposee(p))
        && case_direction(p, &p->pacman, p->pacman.etat.prochaine_direction) != '*')) {
        p->pacman.etat.direction = p->pacman.etat.prochaine_direction;
        p->pacman.etat.prochaine_direction = DIR_INCONNUE;
    }

    // Si pacman est contre un mur alors ne pas bouger
    if (aligne_grille(p, p->pacman.pos) && case_direction(p, &p->pacman, p->pacman.etat.direction) == '*') {
        return;
    }

    const float vitesse = 75;
    switch (p->pacman.etat.direction){
        case DIR_HAUT:   p->pacman.pos.l -= dt * vitesse; break;
        case DIR_BAS:    p->pacman.pos.l += dt * vitesse; break;
        case DIR_GAUCHE: p->pacman.pos.c -= dt * vitesse; break;
        case DIR_DROITE: p->pacman.pos.c += dt * vitesse; break;
    }

    // Animation_time étant un float, on vérifie manuellement si on doit boucler
    p->pacman.animation_time += dt * VITESSE_ANIMATION;
    if (p->pacman.animation_time >= p->pacman.nombre_frames)
        p->pacman.animation_time = 0;

    // Wrapping de pacman sur les bords
    if (p->pacman.pos.c > (p->C - 1) * CASE + CASE && p->pacman.etat.direction == DIR_DROITE) {
        p->pacman.pos.c = -(CASE * 2);
    }
    else if (p->pacman.pos.c < -CASE && p->pacman.etat.direction == DIR_GAUCHE) {
        p->pacman.pos.c = (p->C - 1) * CASE + CASE * 2;
    }
    else if (p->pacman.pos.l > (p->L - 1) * CASE + CASE && p->pacman.etat.direction == DIR_BAS) {
        p->pacman.pos.l = -(CASE * 2);
    }
    else if (p->pacman.pos.l < -CASE && p->pacman.etat.direction == DIR_HAUT) {
        p->pacman.pos.l = (p->L - 1) * CASE + CASE * 2;
    }

    // Vérifie si pacman est en collision avec un bonbon
    if (centre_case(p)) {
        Pos grille = ecran_vers_grille(p->pacman.pos);
        if (on_grid(p, grille.l, grille.c) == '.'){
            p->pacman.etat.score++;
            p->nbbonus--;
            #if DEBUG
            printf("bonbons : %d\n", p->nbbonus);
            #endif
        }
        else if (on_grid(p, grille.l, grille.c) == 'B') {
            p->pacman.etat.score+=50;
            p->pacman.etat.fuite=1;
            p->nbbonus--;
            //metre compteur de temps a 0
            #if DEBUG
            printf("bonbons : %d\n", p->nbbonus);
            #endif
        }
        if (on_grid(p, grille.l, grille.c) != ' ')
            p->plateau[grille.l][grille.c] = 'P';
    }
}

static SDL_Surface* sprite_pacman(Entite *p) {
    // Si pacman n'a pas de direction (arrive lors de la frame frame du chargement du niveau
    if (p->etat.direction == DIR_INCONNUE) return p->sprite[DIR_HAUT][0];
    return p->sprite[p->etat.direction][(int)p->animation_time];
}

void dessiner_pacman(Partie *p) {
    Point pos = {p->pacman.pos.c, p->pacman.pos.l};
    afficher_surface(sprite_pacman(&p->pacman), pos);
}
