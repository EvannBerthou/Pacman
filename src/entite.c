#include "entite.h"

Entite nouvelle_entite(Pos pos, TypeEntite type) {
    Etat etat = {
        .sens = 0, //TODO: Donner un sens de départ aux entités
        .prochain_sens = 0,
        .score = 0,
        .fuite = 0,
        .nb_vie = 3,
    };
    Entite result = {
        .pos = pos,
        .type = type,
        .etat = etat,
        .contre_mur = 0
    };
    return result;
}


Pos ecran_vers_grille(Pos pos, Pos taille) {
    return (Pos) {
        pos.l  / taille.l,
        pos.c  / taille.c,
    };
}