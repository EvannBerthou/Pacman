#include "../lib/libgraphique.h"
#include "entrer.h"

extern SDL_Joystick *manette;

static int touche_manete(int bouton) {
    if (manette == NULL) return 0;
    return SDL_JoystickGetButton(manette, bouton);
}

static Uint8 croix_manette() {
    if (manette == NULL) return 0;
    return SDL_JoystickGetHat(manette, 0);
}

int nouvelle_touche() {
    static int derniere_touche = 0;
    static Uint8 derniere_croix  = 0;
    static int dernier_bouton = 0;

    int touches[] = {SDLK_DOWN, SDLK_UP, SDLK_LEFT, SDLK_RIGHT, SDLK_RETURN, SDLK_q};
    int croix[] = {SDL_HAT_DOWN, SDL_HAT_UP, SDL_HAT_LEFT, SDL_HAT_RIGHT};
    int boutons[] = {0,1};

    int touche = attendre_touche_duree(10);
    Uint8 croix_presse = croix_manette();

    // Détecte le pression des touches entrer et q ou 0 et 1 sur manette tout en évidant la répétition
    int bouton = 0;
    for (int i = 0; i < 2; i++) {
        if (touches[4 + i] != dernier_bouton && (touche == touches[4 + i] || touche_manete(boutons[i]))) {
            bouton = touches[4 + i];
            break;
        }
    }

    // Vérifie si la touche détecté était déjà appuyé lors du dernier appel, évite
    // la répétition de la touche (ou de la croix)
    if (touche == derniere_touche && croix_presse == derniere_croix) return 0;

    derniere_touche = touche;
    derniere_croix = croix_presse;

    // Vérifie les directions
    for (int i = 0; i < 4; i++) {
        if (touche == touches[i] || croix_presse & croix[i]) {
            return touches[i];
        }
    }
    // Aucune touche pressée
    return bouton;
}

int manette_active() {
    return !(manette == NULL);
}
