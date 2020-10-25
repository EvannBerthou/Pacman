#include "pacman.h"
#include "partie.h"

// Renvoie 1 si pacman est aligné à la grille et donc pas en transition
// Pacman est aligné sur la grille ssi ces coordonés sont des multiples de la taille d'une case
static int aligne_grille(Partie *p) {
    return (p->pacman.pos.l % p->tc.x == 0 && p->pacman.pos.c % p->tc.y == 0);
}

//modification de la positon de pacman en fonction de l'environnement et des touches pressés
void bouger_pacman(Partie *p, float dt) {
    // Si pacman ne se déplace dans aucune direciton 
    // Ou qu'il a une direction en attente et qu'il atteint une intersection
    // Alors il change de direction
    if (p->pacman.etat.sens == 0 
        || (aligne_grille(p) && case_direction(p, &p->pacman, p->pacman.etat.prochain_sens) != '*')) {
        p->pacman.etat.sens = p->pacman.etat.prochain_sens;
        p->pacman.etat.prochain_sens = 0;
    }
    
    // Si pacman est contre un mur alors ne pas bouger
    if (aligne_grille(p) && case_direction(p, &p->pacman, p->pacman.etat.sens) == '*') {
            return;
    }

    Pos grille = ecran_vers_grille(p->pacman.pos, (Pos){p->tc.x, p->tc.y});
    p->plateau[grille.l][grille.c] = ' ';

    switch (p->pacman.etat.sens){
        case DIR_HAUT: p->pacman.pos.l-=1; break;
        case DIR_BAS: p->pacman.pos.l+=1; break;
        case DIR_GAUCHE: p->pacman.pos.c-=1; break;
        case DIR_DROITE: p->pacman.pos.c+=1; break;
    }

    grille = ecran_vers_grille(p->pacman.pos, (Pos){p->tc.x, p->tc.y});
    p->plateau[grille.l][grille.c] = 'P';
}

void dessiner_pacman(Partie *p) {
    Point pos = {p->pacman.pos.c, p->pacman.pos.l};
    dessiner_rectangle(pos, p->tc.x, p->tc.y, jaune);
}
