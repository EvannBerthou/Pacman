/******************************************************************************/
/* MAIN.c                                                                     */
/******************************************************************************/
#include <dirent.h>
#include "./main.h"
#include "../lib/libgraphique.h"
#include "timer.h"
#include "entrer.h"
#include "affichage.h"
#include "accueil.h"
#include "audio.h"

/******************************************************************************/
/* MAIN                                                                       */
/******************************************************************************/

SCENE scene_active = SCENE_ACCUEIL;
SDL_Joystick *manette = NULL;

// Le point d'entré n'est pas le même entre un programme Windows et Linux
#ifdef __WIN32
int WinMain(int argc, char **argv) {
#else
int main(int argc, char **argv) {
#endif
    ouvrir_fenetre(ecran_w(), ecran_h());
    SDL_EnableKeyRepeat(0,0); // Désactive la répétition de touche

    // Chargement de la manette si disponible
    int num_joy = SDL_NumJoysticks();

#ifdef DEBUG
    printf("%d manettes détectés\n", num_joy);
#endif

    if (num_joy > 0) {
        printf("Utilisation de la manette 0 (%s)\n", SDL_JoystickName(0));
        // Active la manette
        manette = SDL_JoystickOpen(0);
        // Active la mise à jour des evenements de la manette (sera appelé dans traiter_evenement
        // par la libgraphique)
        SDL_JoystickEventState(SDL_ENABLE);
    }

    init_sons(); // Chargement des sons
    charger_fichier_audio(0); // Joue la musique de l'écran d'accueil

    // Création des boutons
    if (charger_sprites() == -1)
        return 1;

    charger_boutons();
    Timer timer = nouveau_timer();
    Partie p = {0};
    // Boucle principale
    // On peut faire une boucle infinie car lorsque le signal d'arrêt est envoyé, la libgraphique
    // termine directement le programme, toute la mémoire alloué est directement libérée par le système
    while(1) {
        tick_timer(&timer);
#ifdef DEBUG
        print_fps(&timer);
#endif
        traiter_evenements();
        actualiser_jeu(&p, &timer);
        dessiner_jeu(&p);
        actualiser();
        reinitialiser_evenements();
    }
    return 0;
}

// Actualise la scene active
void actualiser_jeu(Partie *p, Timer *t) {
    switch(scene_active) {
    case SCENE_ACCUEIL:
        actualiser_accueil(p, t);
        break;
    case SCENE_NIVEAU:
        actualiser_partie(p, t);
        break;
    }
}

// Dessine la scene active
void dessiner_jeu(Partie *p) {
    switch(scene_active) {
    case SCENE_ACCUEIL:
        dessiner_accueil();
        break;
    case SCENE_NIVEAU:
        dessiner_partie(p);
        break;
    }
}

void changer_scene(SCENE s) {
    scene_active = s;
    switch(s) {
        case SCENE_ACCUEIL: charger_accueil();
        default: break; // Ne rien faire de spécial
    }
}