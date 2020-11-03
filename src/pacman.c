#include "pacman.h"
#include "partie.h"

#define VITESSE_ANIMATION 7

// Renvoie 1 si pacman est aligné à la grille et donc pas en transition
// Pacman est aligné sur la grille ssi ces coordonés sont des multiples de la taille d'une case
static int aligne_grille(Partie *p) {
    return (p->pacman.pos.l % p->tc.l == 0 && p->pacman.pos.c % p->tc.c == 0);
}

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
    int demi_x = p->tc.l / 2;
    int demi_y = p->tc.c / 2;

// Necessaire pour les cas spécials dans les coins
// TODO: Sûrement une meilleure solution disponible
    switch (p->pacman.etat.direction){
        case DIR_HAUT:
        case DIR_GAUCHE: return (p->pacman.pos.l + demi_x) % p->tc.l > demi_x || (p->pacman.pos.c + demi_y) % p->tc.c > demi_y;
        case DIR_BAS:    return (p->pacman.pos.l + demi_x) % p->tc.l > demi_x || (p->pacman.pos.c + demi_y) % p->tc.c >= demi_y;
        case DIR_DROITE: return (p->pacman.pos.l + demi_x) % p->tc.l >= demi_x || (p->pacman.pos.c + demi_y) % p->tc.c > demi_y;
    }
    return 0;
}

//modification de la positon de pacman en fonction de l'environnement et des touches pressés
void bouger_pacman(Partie *p, float dt) {
    // Si pacman ne se déplace dans aucune direciton 
    // Ou qu'il a une direction en attente et qu'il atteint une intersection
    // Ou qu'il part dans dans la direction opposée
    // Alors il change de direction
    if (p->pacman.etat.direction == DIR_INCONNUE
        || ((aligne_grille(p) || direction_opposee(p))
        && case_direction(p, &p->pacman, p->pacman.etat.prochaine_direction) != '*')) {
        p->pacman.etat.direction = p->pacman.etat.prochaine_direction;
        p->pacman.etat.prochaine_direction = DIR_INCONNUE;
    }
    
    // Si pacman est contre un mur alors ne pas bouger
    if (aligne_grille(p) && case_direction(p, &p->pacman, p->pacman.etat.direction) == '*') {
        return;
    }

    switch (p->pacman.etat.direction){
        case DIR_HAUT: p->pacman.pos.l-=1; break;
        case DIR_BAS: p->pacman.pos.l+=1; break;
        case DIR_GAUCHE: p->pacman.pos.c-=1; break;
        case DIR_DROITE: p->pacman.pos.c+=1; break;
    }

    p->pacman.animation_time += dt * VITESSE_ANIMATION;
    if (p->pacman.animation_time >= p->pacman.nombre_frames)
        p->pacman.animation_time = 0;

    // Wrapping de pacman sur les bords
    // Droite
    if (p->pacman.pos.c > (p->C - 1) * p->tc.c + p->tc.c && p->pacman.etat.direction == DIR_DROITE) { 
        p->pacman.pos.c = -(p->tc.c * 2);
    }
    // Gauche
    else if (p->pacman.pos.c < -p->tc.c && p->pacman.etat.direction == DIR_GAUCHE) { 
        p->pacman.pos.c = (p->C - 1) * p->tc.c + p->tc.c * 2;
    }
    // TODO: Segfault quand on sors du plateau par en bas
    // Bas
    else if (p->pacman.pos.c > (p->C - 1) + p->tc.l * p->tc.c && p->pacman.etat.direction == DIR_BAS) { 
        p->pacman.pos.l = -(p->tc.l * 2);
    }
    // Haut
    else if (p->pacman.pos.l < -p->tc.l && p->pacman.etat.direction == DIR_HAUT) { 
        p->pacman.pos.l = (p->C - 1) * p->tc.l + p->tc.l * 2;
    }

    // Vérifie si pacman est en collision avec un bonbon
    if (centre_case(p)) {
        Pos grille = ecran_vers_grille(p->pacman.pos, p->tc);
        if (p->plateau[grille.l][grille.c]=='.'){
            p->pacman.etat.score++;
            p->nbbonus--;
            #if DEBUG 
            printf("bonbons : %d\n", p->nbbonus);
            #endif
        }
        else if (p->plateau[grille.l][grille.c]=='B') {
            p->pacman.etat.score+=50;
            p->pacman.etat.fuite=1;
            p->nbbonus--;
            //metre compteur de temps a 0
            #if DEBUG 
            printf("bonbons : %d\n", p->nbbonus);
            #endif
        }
        p->plateau[grille.l][grille.c] = 'P';
    }
}

void dessiner_pacman(Partie *p) {
    Point pos = {p->pacman.pos.c, p->pacman.pos.l};
    afficher_surface(p->pacman.sprite[p->pacman.etat.direction][(int)p->pacman.animation_time], pos);
}
