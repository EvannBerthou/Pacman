#include <math.h>
#include "entite.h"
#include "main.h"
#include "fantome.h"

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
        .pos_cible=(Posf){0,0},
        .type = type,
        .etat = etat,
        .animation_time = 0,
        .nombre_noeud=0,
        .chemin_noeud={0},
    };

    if (type == ENTITE_PACMAN) {
        result.vitesse = 75;
    }// Pacman (pas ENTITE_FANTOME) possède 2 frames d'animation : bouche ouverte/fermée
    else {
        result.vitesse = 50;
    }

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
