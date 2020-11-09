#include "fantome.h"

void bouger_fantomes(Partie *p, float dt) {
    Posf pacman = p->pacman.pos;
    for (int i = 0; i < NBFANTOMES; i++) {
        Entite *fantome = &p->fantomes[i];
        // Verifie quelle direction est disponible pour le fantôme
        DirEntite direction = DIR_INCONNUE;
        if (fantome->pos.l > pacman.l && case_direction(p, fantome, DIR_HAUT) != '*')
            direction = DIR_HAUT;
        else if (fantome->pos.l < pacman.l && case_direction(p, fantome, DIR_BAS) != '*')
            direction = DIR_BAS;
        else if (fantome->pos.c > pacman.c && case_direction(p, fantome, DIR_GAUCHE) != '*')
            direction = DIR_GAUCHE;
        else if (fantome->pos.c < pacman.c && case_direction(p, fantome, DIR_DROITE) != '*')
            direction = DIR_DROITE;

        // Change la direction du fantôme ssi il est aligné à la grille pour éviter qu'il traverse les murs
        if (aligne_grille(p, fantome->pos)) {
            fantome->etat.direction = direction;
        }

        // Déplace le fantôme
        const float vitesse = 50;
        switch (fantome->etat.direction) {
            case DIR_HAUT:   fantome->pos.l-= dt * vitesse; break;
            case DIR_BAS:    fantome->pos.l+= dt * vitesse; break;
            case DIR_GAUCHE: fantome->pos.c-= dt * vitesse; break;
            case DIR_DROITE: fantome->pos.c+= dt * vitesse; break;
        }
    }
}

void dessiner_fantomes(Partie *p) {
    for (int i = 0; i < NBFANTOMES; i++) {
        Entite *fantome = &p->fantomes[i];
        Point pos = {fantome->pos.c, fantome->pos.l};
        if (fantome->etat.direction == DIR_INCONNUE) 
            afficher_surface(fantome->sprite[1][0], pos);
        else
            afficher_surface(fantome->sprite[fantome->etat.direction][0], pos);
    }
}