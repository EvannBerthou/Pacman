#include "pacman.h"

static char case_direction(Partie *p, int sens) {
    Pos pos = p->pacman.pos;
    switch(sens) {
    case 1: return p->plateau[pos.l-1][pos.c];
    case 2: return p->plateau[pos.l+1][pos.c];
    case 3: return p->plateau[pos.l][pos.c-1];
    case 4: return p->plateau[pos.l][pos.c+1];
    }
    return '*';
}

//modification de L et C de pacman en fonction de l'environemen et des touche préssées
void bouger_pacman(Partie *p) {
    if (p->pacman.etat.sens == 0 || case_direction(p, p->pacman.etat.prochain_sens) != '*') {
        p->pacman.etat.sens = p->pacman.etat.prochain_sens;
        p->pacman.etat.prochain_sens = 0;
    }

    if (p->pacman.delai_deplacement > 0) return;
    if (case_direction(p, p->pacman.etat.sens) == '*') return; // Si pacman se dirige vers un mur

    // Supprime l'ancienne position de pacman
    p->plateau[p->pacman.pos.l][p->pacman.pos.c] = ' ';

    switch (p->pacman.etat.sens){
        case 1: p->pacman.pos.l-=1; break;
        case 2: p->pacman.pos.l+=1; break;
        case 3: p->pacman.pos.c-=1; break;
        case 4: p->pacman.pos.c+=1; break;
    }
    // le cas ou il sort par la droite et revien par la gauche
    if (p->pacman.pos.c>20 && p->pacman.etat.sens==4){
        p->pacman.pos.c=p->C;
    }
    // le cas ou il sort par la gauche et revien par la droite
    else  if (p->pacman.pos.c<0 && p->pacman.etat.sens==3){
        p->pacman.pos.c=p->L;
    }

    if (p->pacman.delai_deplacement < 0) {
        p->pacman.delai_deplacement = PACMAN_DELAY;
    }

    // Applique les changements de position au plateau
    p->plateau[p->pacman.pos.l][p->pacman.pos.c] = 'P';

}
