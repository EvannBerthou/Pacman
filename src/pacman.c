#include "pacman.h"
#include "partie.h"

// Renvoie 1 si pacman est aligné à la grille et donc pas en transition
// Pacman est aligné sur la grille ssi ces coordonés sont des multiples de la taille d'une case
static int aligne_grille(Partie *p) {
    return (p->pacman.pos.l % p->tc.l == 0 && p->pacman.pos.c % p->tc.c == 0);
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
void bouger_pacman(Partie *p) {
    // Si pacman ne se déplace dans aucune direciton 
    // Ou qu'il a une direction en attente et qu'il atteint une intersection
    // Alors il change de direction
    if (p->pacman.etat.direction == 0 
        || (aligne_grille(p) && case_direction(p, &p->pacman, p->pacman.etat.prochain_sens) != '*')) {
        p->pacman.etat.direction = p->pacman.etat.prochain_sens;
        p->pacman.etat.prochain_sens = 0;
    }
    
    // Si pacman est contre un mur alors ne pas bouger
    if (aligne_grille(p) && case_direction(p, &p->pacman, p->pacman.etat.direction) == '*') {
        return;
    }

    // Vérifie si pacman est en collision avec un bonbon
    int manger = centre_case(p);

    if (manger) {
        Pos grille = ecran_vers_grille(p->pacman.pos, p->tc);
        p->plateau[grille.l][grille.c] = ' ';
    }

    switch (p->pacman.etat.direction){
        case DIR_HAUT: p->pacman.pos.l-=1; break;
        case DIR_BAS: p->pacman.pos.l+=1; break;
        case DIR_GAUCHE: p->pacman.pos.c-=1; break;
        case DIR_DROITE: p->pacman.pos.c+=1; break;
    }

    if (manger) {
        Pos grille = ecran_vers_grille(p->pacman.pos, p->tc);
        p->plateau[grille.l][grille.c] = 'P';
    }
}

void dessiner_pacman(Partie *p) {
    Point pos = {p->pacman.pos.c, p->pacman.pos.l};
    dessiner_rectangle(pos, p->tc.l, p->tc.c, bleu);
}
