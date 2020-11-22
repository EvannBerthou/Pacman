#include <stdlib.h>
#include "audio.h"

// Fonction d'aide pour gérer les problèmes d'allocation de mémoire
static void *merr(void *p) {
    if (p == NULL) {
        fprintf(stderr, "Erreur malloc dans audio.c\n");
        exit(1);
    }
    return p;
}

// Le code lié à l'audio est inspiré la doc de la SDL1
static SDL_AudioSpec wav_spec; // Caractéristique du fichier audio
static Sons playing = {.data = NULL, .len = 0, .max_size = 10}; // Liste des fichier audios en cours de lecture
static Sons samples; // Liste de tous les fichiers
static int _volume = SDL_MIX_MAXVOLUME;
static int _audio_actif = 1;

// Liste des fichiers audio à charger
#define SAMPLES_COUNT 6
const char *sons_chemin[SAMPLES_COUNT] = {
    "data/sons/music.wav",
    "data/sons/pacman_chomp.wav",
    "data/sons/pacman_eatghost.wav",
    "data/sons/pacman_eatfruit.wav",
    "data/sons/pacman_beginning.wav",
    "data/sons/pacman_death.wav",
};
// Le fichier doit-il être rejoué lorsqu'il est fini ? 
// Correspond au même que sons_chemin
const int looping[SAMPLES_COUNT] = {
    1, 1, 0, 0, 0, 0
};

// Permet d'avancer la lecture du fichier audio
// Provient de la doc SDL1
static void retour_audio(void *userdata, Uint8 *stream, int len) {
    Sons *sons = (Sons*) userdata;

    for (int i = 0; i < sons->len; i++) {
        Son *s = sons->data[i];
        if (!s->playing) {
            continue;
        }
        if (s->curr >= s->len) {
            if (s->loop) {
                s->curr = 0;
            }
            else {
                s->playing = 0;
                continue;
            }
        }
        // Si le temps restant est plus petit que le temps à jouer, alors ne jouer que le temps restant:
        len = ( s->curr + len > s->len ? s->len - s->curr : len);
        // Additionne les sons en cours de lecture
        SDL_MixAudio(stream, s->buff + s->curr, len, _volume);
        // Fais avancer l'audio
        s->curr += len;
    }
}

// Chargement des sons
void init_sons() {
    // Initialisation des de playing
    playing.data = merr(malloc(sizeof(Son*) * playing.max_size));
    for (int i = 0; i < playing.max_size; i++) {
        playing.data[i] = merr(malloc(sizeof(Son)));
    }

    samples.data = merr(malloc(sizeof(Son*) * SAMPLES_COUNT));
    for (int i = 0; i < SAMPLES_COUNT; i++) {
        samples.data[i] = merr(calloc(1, sizeof(Son)));
    }
    // Charge chaque fichier dans samples
    samples.len = SAMPLES_COUNT;
    for (int i = 0; i < SAMPLES_COUNT; i++) {
        if (SDL_LoadWAV(sons_chemin[i], &wav_spec, &samples.data[i]->buff, &samples.data[i]->len) != NULL ){
            samples.data[i]->loop = looping[i];
            wav_spec.callback = retour_audio;
            wav_spec.userdata = &playing;
        }
        else {
            fprintf(stderr, "erreur chargement %s\n", sons_chemin[i]);
        }
    }

    // Ouverture du périphérique audio
    if (SDL_OpenAudio(&wav_spec, NULL) < 0 ) {
    #ifdef DEBUG
        fprintf(stderr, "Erreur lors de l'ouverture de l'audio: %s\n", SDL_GetError());
    #endif
        _audio_actif = 0;
        return;
    }
    // Active la lecture du son
    SDL_PauseAudio(0);
}

// Redimensionne playing et inialise les nouveaux éléments
static void resize_playing() {
    playing.max_size *= 2;
    playing.data = merr(realloc(playing.data, sizeof(Son*) * playing.max_size));
    for (int i = playing.len; i < playing.max_size; i++) {
        playing.data[i] = merr(calloc(1, sizeof(Son)));
    }
}

// Ajoute un audio à playing pour être ensuite joué
int charger_fichier_audio(int id) {
    // Si playing est complet, augmenter sa taille
    if (playing.len == playing.max_size) {
        resize_playing();
    }
    // Copie l'audio de samples vers playing
    memcpy(playing.data[playing.len], samples.data[id], sizeof(Son));
    playing.data[playing.len]->playing = 1;
    playing.len++;
    // Renvoie l'index du nouveau son
    return playing.len - 1;
}

// Mettre en pause le son ou non
void pause_son(int index, int pause) {
    playing.data[index]->playing = !pause;
}

// Le fichier est-il en cours de lecture ?
int is_playing(int index) {
    return playing.data[index]->playing;
}

// Remet un son au début et en pause
void stop_son(int index) {
    playing.data[index]->playing = 0;
    playing.data[index]->curr = 0;
}

// Active et désactive le son lorsque m est pressé
void toggle_volume(int touche) {
    if (touche == SDLK_m) {
        _volume = (_volume == 0) ? SDL_MIX_MAXVOLUME : 0; // Echange le volume entre volume max et coupé
    }
}

int volume() {
    return _volume;
}

int audio_actif() {
    return _audio_actif;
}
