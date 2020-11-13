#include <unistd.h>
#include <math.h>
#include "fantome.h"
#include "main.h"

void bouger_fantomes(Partie *p, float dt) {
    for (int i = 0; i < NBFANTOMES; i++) {
        Entite *fantome = &p->fantomes[i];
        Pos current_pos = ecran_vers_grille(fantome->pos);

        // Si le fantome est rentré à la base
        // TODO: Détecte que le fantome est à la base
        if (fantome->etat.fuite && ((fantome->pos.l == fantome->pos_init.l) && (fantome->pos.l == fantome->pos_init.l))){
            fantome->etat.fuite=0;
        }

        // Si le fantome est en mode fuite, le faire se déplacer vers sa base
        if (fantome->etat.fuite){
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
