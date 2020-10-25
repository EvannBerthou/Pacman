#include "fantome.h"

// Fait bouger chaque fantôme dans la direction de pacman
void bouger_fantomes(Partie *p) {
    Pos pacman = p->pacman.pos;
    for (int i = 0; i < NBFANTOMES; i++) {
        Entite *fantome = &p->fantomes[i];
        Pos grille = ecran_vers_grille(fantome->pos, (Pos){p->tc.x, p->tc.y});
        p->plateau[grille.l][grille.c] = ' ';

        if (fantome->pos.l > pacman.l && case_direction(p, fantome, DIR_HAUT) != '*') {
            fantome->pos.l -= p->tc.x;
        }
        else if (fantome->pos.l < pacman.l && case_direction(p, fantome, DIR_BAS) != '*') {
            fantome->pos.l += p->tc.x;
        }
        else if (fantome->pos.c > pacman.c && case_direction(p, fantome, DIR_GAUCHE) != '*') {
            fantome->pos.c -= p->tc.y;
        }
        else if (fantome->pos.c < pacman.c && case_direction(p, fantome, DIR_DROITE) != '*') {
            fantome->pos.c += p->tc.y;
        }

        grille = ecran_vers_grille(fantome->pos, (Pos){p->tc.x, p->tc.y});
        p->plateau[grille.l][grille.c] = 'F';
    }

    return;
}
