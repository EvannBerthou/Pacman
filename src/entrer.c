#include "../lib/libgraphique.h"
#include "entrer.h"
#include "audio.h"

extern SDL_Joystick *manette;

static int touche_manette(int bouton) {
    if (manette == NULL) return 0;
    return SDL_JoystickGetButton(manette, bouton);
}

static Uint8 croix_manette() {
    if (manette == NULL) return 0;
    return SDL_JoystickGetHat(manette, 0);
}

#define NOMBRE_BOUTON 4
static void boutons_manette(int *res) {
    for (int i = 0; i < NOMBRE_BOUTON; i++) {
        res[i] = touche_manette(i);
    }
}

int nouvelle_touche() {
    static Uint8 derniere_croix  = 0;
    static int derniers_boutons[NOMBRE_BOUTON] = {0,0,0,0};
    static int derniere_touche = 0;

    #define NOMBRE_DIRECTIONS 4
    #define NOMBRE_TOUCHES 5
    int touches[] = {SDLK_DOWN, SDLK_UP, SDLK_LEFT, SDLK_RIGHT, SDLK_RETURN, SDLK_q, SDLK_b, SDLK_a, SDLK_m};
    int croix[] = {SDL_HAT_DOWN, SDL_HAT_UP, SDL_HAT_LEFT, SDL_HAT_RIGHT};

    Uint8 croix_presse = croix_manette();

    // Vérifie les directions
    for (int i = 0; i < NOMBRE_DIRECTIONS; i++) {
        if (touche_a_ete_pressee(touches[i]) || croix_presse & croix[i]) {
            if ((derniere_croix & croix[i]) > 0) {
                continue;
            }
            avancer_konami_code(touches[i]);
            derniere_croix = croix_presse;
            return touches[i];
        }
    }
    derniere_croix = croix_presse;

    for (int i = 0; i < NOMBRE_TOUCHES; i++) {
        if (touche_a_ete_pressee(touches[4 + i])) {
            if (touches[4 + i] == derniere_touche) {
                continue;
            }
            derniere_touche = touches[4 + i];
            avancer_konami_code(touches[4 + i]);
            return touches[4 + i];
        }
    }

    // Boutons de la manette pressé cette frame
    int boutons[NOMBRE_BOUTON];
    boutons_manette(boutons);

    // Détermine la nouvelle touche de la manette pressée
    int res_bouton = 0;
    for (int i = 0; i < NOMBRE_BOUTON; i++) {
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
    if (res_bouton != 0) {
        avancer_konami_code(res_bouton);
        return res_bouton;
    }
    derniere_touche = 0;
    return 0;
}

int manette_active() {
    return !(manette == NULL);
}

static const int konami_code[] = {SDLK_UP, SDLK_UP, SDLK_DOWN, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT, SDLK_LEFT, SDLK_RIGHT, SDLK_b, SDLK_a};
void avancer_konami_code(int touche) {
    static int index = 0;
    if (touche == konami_code[index + 1]) {
        index++;
        if (index == 9) {
            printf("Code complet !\n");
            index = 0;
        }
    }
    else if (touche == konami_code[0]) {
        index = 1;
    }
    else {
        index = 0;
    }
}

void attendre_sortie() {
    int touche = 0;
    while (touche != SDLK_q) {
        traiter_evenements();
        touche = nouvelle_touche();
        toggle_volume(touche);
        reinitialiser_evenements();
    }
}
