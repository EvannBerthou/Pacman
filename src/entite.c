#include <math.h>
#include "entite.h"
#include "main.h"
#include "fantome.h"
#include "pacman.h"

// Création d'une nouvelle entitée
Entite nouvelle_entite(Posf pos, Posf pos_init, TypeEntite type) {
    Etat etat = {
        .direction = DIR_INCONNUE,
        .prochaine_direction = DIR_INCONNUE,
        .score = 0,
        .fuite = 0,
        .nb_vie = 2,
        .mange = 0,
        .suiv = 0,
    };
    Entite result = {
        .pos = pos,
        .pos_init = pos_init,
        .pos_cible=(Pos){0,0},
        .type = type,
        .etat = etat,
        .animation_time = 0,
        .nombre_noeud=0,
        .prochain_noeud = NULL,
    };

    // Détermine la vitesse de l'entité en fonction de son type
    result.vitesse = (type == ENTITE_PACMAN) ? VITESSE_PACMAN : VITESSE_FANTOME; 

    return result;
}

Pos ecran_vers_grille(Posf pos) {
    // Convertis taille en Posf pour ne pas perdre la précision lors de la division
    Posf taille_f = { CASE, CASE };
    return (Pos) {
        (int)(roundf (pos.l  / taille_f.l)),
        (int)(roundf (pos.c  / taille_f.c))
    };
}
