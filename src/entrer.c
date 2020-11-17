#include "../lib/libgraphique.h"
#include "entrer.h"

extern SDL_Joystick *manette;

static int touche_manette(int bouton) {
    if (manette == NULL) return 0;
    return SDL_JoystickGetButton(manette, bouton);
}

static Uint8 croix_manette() {
    if (manette == NULL) return 0;
    return SDL_JoystickGetHat(manette, 0);
}

static void boutons_manette(int *res) {
    res[0] = touche_manette(0);
    res[1] = touche_manette(1);
}

int nouvelle_touche() {
    static int derniere_touche = 0;
    static Uint8 derniere_croix  = 0;
    static int derniers_boutons[2] = {0,0};

    int touches[] = {SDLK_DOWN, SDLK_UP, SDLK_LEFT, SDLK_RIGHT, SDLK_RETURN, SDLK_q};
    int croix[] = {SDL_HAT_DOWN, SDL_HAT_UP, SDL_HAT_LEFT, SDL_HAT_RIGHT};

    int touche = attendre_touche_duree(10);
    Uint8 croix_presse = croix_manette();
    // Boutons de la manette pressé cette frame
    int boutons[2];
    boutons_manette(boutons);


    // Détermine la nouvelle touche de la manette pressée
    int res_bouton = 0;
    for (int i = 0; i < 2; i++) {
        if (boutons[i] && derniers_boutons[i] == 0) {
            // Sauvegarde quelle nouvelle touche de la manette a été pressé sous la forme
            // d'une touche de clavier
            // ex : Rond sur la manette va renvoyer SDLK_q
            res_bouton = touches[4 + i];
        }
        // Copie l'état actuel dans l'état ancien
        derniers_boutons[i] = boutons[i];
    }
    // Si une touche de la manette a été pressé pour la première fois, la renvoyée
    if (res_bouton != 0) return res_bouton;

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

    return touche;
}

int manette_active() {
    return !(manette == NULL);
}
