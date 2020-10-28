#include "entite.h"

Entite nouvelle_entite(Pos pos, TypeEntite type) {
    Etat etat = {
        .direction = 0, //TODO: Donner une direction de départ aux entités
        .prochaine_direction = 0,
        .score = 0,
        .fuite = 0,
        .nb_vie = 3,
    };
    Entite result = {
        .pos = pos,
        .type = type,
        .etat = etat,
    };
    return result;
}

Pos ecran_vers_grille(Pos pos, Pos taille) {
    return (Pos) {
        pos.l  / taille.l,
        pos.c  / taille.c,
    };
}