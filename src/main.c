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

/******************************************************************************/
/* MAIN                                                                       */
/******************************************************************************/

SCENE scene_active = SCENE_ACCUEIL;
SDL_Joystick *manette = NULL;

static Uint8 *audio_pos; // avance de la lecture du fichier audio
static Uint32 audio_len; // taille restante à être lue

// Permet d'avancer la lecture du fichier audio
// Provient de la doc SDL1
void my_audio_callback(void *userdata, Uint8 *stream, int len) {
	if (audio_len == 0)
		return;

	len = ( len > audio_len ? audio_len : len );
	SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);// mix from one buffer into another

	audio_pos += len;
	audio_len -= len;
}

int main(int argc, char **argv) {
    ouvrir_fenetre(ECRAN_W, ECRAN_H);

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


    // Le code lié à l'audio est directement pris depuis la doc de la SDL1
	static Uint32 wav_length; // taille du fichier audio
	static Uint8 *wav_buffer; // contenue du fichier audio
	static SDL_AudioSpec wav_spec; // Caractéristique du fichier audio

    // Charge le fichier musique
	if (SDL_LoadWAV("music.wav", &wav_spec, &wav_buffer, &wav_length) != NULL ){
        wav_spec.callback = my_audio_callback;
        wav_spec.userdata = NULL;
        audio_pos = wav_buffer;
        audio_len = wav_length;

        // Ouvre le périphérique audio
        if (SDL_OpenAudio(&wav_spec, NULL) < 0 ){
        #ifdef DEBUG
          fprintf(stderr, "Erreur lors de l'ouverture de l'audio: %s\n", SDL_GetError());
        #endif
        }
        else {
            SDL_PauseAudio(0);
        }
    }

    // Création des boutons
    if (charger_sprites() == -1)
        return 1;

    charger_boutons();
    Timer timer = nouveau_timer();
    Partie p;
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
