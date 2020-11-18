#include <stdlib.h>
#include "audio.h"

// Le code lié à l'audio est directement pris depuis la doc de la SDL1
static SDL_AudioSpec wav_spec; // Caractéristique du fichier audio
static Sons playing;
static Sons samples;

#define SAMPLES_COUNT 2
const char *sons_chemin[SAMPLES_COUNT] = {
    "music.wav",
    "pacman_chomp.wav"
};

// Permet d'avancer la lecture du fichier audio
// Provient de la doc SDL1
static void retour_audio(void *userdata, Uint8 *stream, int len) {
    Sons *sons = (Sons*) userdata;

    for (int i = 0; i < sons->len; i++) {
        Son *s = sons->data[i];
        if (s->curr == s->len) {
            continue;
        }
        len = ( s->curr + len > s->len ? s->len - s->curr : len);
        SDL_MixAudio(stream, s->buff + s->curr, len, SDL_MIX_MAXVOLUME);
        s->curr += len;
    }
}

void init_sons() {
    playing.data = malloc(sizeof(Son*) * 10);
    for (int i = 0; i < 10; i++) {
        playing.data[i] = malloc(sizeof(Son));
    }
    playing.len = 0;

    samples.data = malloc(sizeof(Son*) * SAMPLES_COUNT);
    for (int i = 0; i < SAMPLES_COUNT; i++) {
        samples.data[i] = calloc(1, sizeof(Son));
    }
    samples.len = SAMPLES_COUNT;
    for (int i = 0; i < SAMPLES_COUNT; i++) {
        if (SDL_LoadWAV(sons_chemin[i], &wav_spec, &samples.data[i]->buff, &samples.data[i]->len) != NULL ){
            wav_spec.callback = retour_audio;
            wav_spec.userdata = &playing;
        }
        else {
            fprintf(stderr, "erreur chargement %s\n", sons_chemin[i]);
        }
    }

    if (SDL_OpenAudio(&wav_spec, NULL) < 0 ) {
    #ifdef DEBUG
      fprintf(stderr, "Erreur lors de l'ouverture de l'audio: %s\n", SDL_GetError());
    #endif
      return;
    }
    SDL_PauseAudio(0);
}

void charger_fichier_audio(int id) {
    memcpy(playing.data[playing.len++], samples.data[id], sizeof(Son));
}
