#include "fantome.h"

// Fait bouger chaque fantôme dans la direction de pacman
void bouger_fantomes(Partie *p) {
    Pos pacman = p->pacman.pos;
    for (int i = 0; i < NBFANTOMES; i++) {
        Entite *fantome = &p->fantomes[i];
        if (fantome->delai_deplacement > 0) return;
        p->plateau[fantome->pos.l][fantome->pos.c] = ' ';

        if (fantome->pos.l > pacman.l && case_direction(p, fantome, DIR_HAUT) != '*') {
            fantome->pos.l--;
        }
        else if (fantome->pos.l < pacman.l && case_direction(p, fantome, DIR_BAS) != '*') {
            fantome->pos.l++;
        }
        else if (fantome->pos.c > pacman.c && case_direction(p, fantome, DIR_GAUCHE) != '*') {
            fantome->pos.c--;
        }
        else if (fantome->pos.c < pacman.c && case_direction(p, fantome, DIR_DROITE) != '*') {
            fantome->pos.c++;
        }
        p->plateau[fantome->pos.l][fantome->pos.c] = 'F';

        if (fantome->delai_deplacement < 0) {
            fantome->delai_deplacement = PACMAN_DELAY;
        }
    }

    return;
}
