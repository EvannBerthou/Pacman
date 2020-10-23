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
        .etat = etat
    };
    return result;
}
