#include "pacman.h"
#include "partie.h"

//modification de L et C de pacman en fonction de l'environemen et des touche préssées
void bouger_pacman(Partie *p) {
    if (p->pacman.etat.sens == 0 || case_direction(p, &p->pacman, p->pacman.etat.prochain_sens) != '*') {
        p->pacman.etat.sens = p->pacman.etat.prochain_sens;
        p->pacman.etat.prochain_sens = 0;
    }

    if (p->pacman.delai_deplacement > 0) return;
    if (case_direction(p, &p->pacman, p->pacman.etat.sens) == '*') {  // Si pacman se dirige vers un mur
        p->pacman.contre_mur = 1;
        return;
    }


    p->pacman.contre_mur = 0;
    // Supprime l'ancienne position de pacman
    p->plateau[p->pacman.pos.l][p->pacman.pos.c] = ' ';

    switch (p->pacman.etat.sens){
        case DIR_HAUT: p->pacman.pos.l-=1; break;
        case DIR_BAS: p->pacman.pos.l+=1; break;
        case DIR_GAUCHE: p->pacman.pos.c-=1; break;
        case DIR_DROITE: p->pacman.pos.c+=1; break;
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

void dessiner_pacman(Partie *p) {
    int cx = p->taille_case[0];
    int cy = p->taille_case[1];
    Point pos = {p->pacman.pos.c * cx, p->pacman.pos.l * cy};
    if (!p->pacman.contre_mur) { 
        float offset = -((float)p->pacman.delai_deplacement / PACMAN_DELAY);
        switch(p->pacman.etat.sens) {
        case DIR_HAUT:   pos.y -= cy * offset; break;
        case DIR_BAS:    pos.y += cy * offset; break;
        case DIR_GAUCHE: pos.x -= cx * offset; break;
        case DIR_DROITE: pos.x += cx * offset; break;
        default: break;
        }
    }
    dessiner_rectangle(pos, cx, cy, jaune);
}
