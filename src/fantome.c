#include <unistd.h>
#include <math.h>
#include "fantome.h"
#include "main.h"

void bouger_fantomes(Partie *p, float dt) {
    for (int i = 0; i < NBFANTOMES; i++) {
        Entite *fantome = &p->fantomes[i];
        Pos current_pos = ecran_vers_grille(fantome->pos);
        Pos pos_init_f= ecran_vers_grille(fantome->pos_init);
        // Si le fantome est rentré à la base
        // TODO: Détecte que le fantome est à la base
        if (((current_pos.l == pos_init_f.l) && (current_pos.c == pos_init_f.c)) && ( fantome->etat.manger|| fantome->etat.fuite)){
            fantome->etat.manger=0;
            fantome->etat.fuite=0;
        }
        if  (fantome->etat.tmp_fuite>=10000.0)
            fantome->etat.fuite=0;

        // Si le fantome est en mode fuite, le faire se déplacer vers sa base
        if (fantome->etat.fuite ){
            fantome->etat.tmp_fuite+=dt;
            find_path(p,current_pos,ecran_vers_grille(fantome->pos_init),fantome);
        }
        else if (fantome->etat.manger){
            find_path(p,current_pos,ecran_vers_grille(fantome->pos_init),fantome);
        }
        else {
            // Ici on gère les différentes IA de chaque fantome car ils ont une case cible differente
            if (fantome->type==ENTITE_FANTOME_R){
                fantome->pos_cible = p->pacman.pos;
                find_path(p,current_pos,ecran_vers_grille(p->pacman.pos),fantome);
            }
        }

        DirEntite direction = DIR_INCONNUE;
        if (fantome->chemin_noeud[0] != NULL) {
            if (current_pos.l > fantome->chemin_noeud[0]->pos.l) {
                direction = DIR_HAUT;
            }
            else if (current_pos.l < fantome->chemin_noeud[0]->pos.l) {
                direction = DIR_BAS;
            }
            else if (current_pos.c > fantome->chemin_noeud[0]->pos.c) {
                direction = DIR_GAUCHE;
            }
            else if (current_pos.c < fantome->chemin_noeud[0]->pos.c) {
                direction = DIR_DROITE;
            }
            // Change la direction du fantôme ssi il est aligné à la grille pour éviter qu'il traverse les murs
            if (aligne_grille(p, fantome->pos)) {
                fantome->etat.direction = direction;
            }
        }
        else {
            direction = fantome->etat.direction;
        }

        // Déplace le fantôme dt * vitesse
        const float vitesse = 50;
        switch (fantome->etat.direction) {
            case DIR_HAUT:   fantome->pos.l -= dt * vitesse; break;
            case DIR_BAS:    fantome->pos.l += dt * vitesse; break;
            case DIR_GAUCHE: fantome->pos.c -= dt * vitesse; break;
            case DIR_DROITE: fantome->pos.c += dt * vitesse; break;
        }
    }
}
/*
static SDL_Surface* sprite_fantome(Entite *p) {
    // Si pacman n'a pas de direction (arrive lors de la frame frame du chargement du niveau
    if (p->etat.direction == DIR_INCONNUE) return p->sprite[DIR_HAUT][0];
    return p->sprite[p->etat.direction][(int)p->animation_time];
}
*/

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
