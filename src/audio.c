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
const int looping[SAMPLES_COUNT] = {
    1, 1
};

// Permet d'avancer la lecture du fichier audio
// Provient de la doc SDL1
static void retour_audio(void *userdata, Uint8 *stream, int len) {
    Sons *sons = (Sons*) userdata;

    for (int i = 0; i < sons->len; i++) {
        Son *s = sons->data[i];
        if (!s->playing) continue;
        if (s->curr >= s->len) {
            if (s->loop) {
                s->curr = 0;
            }
            else {
                s->playing = 0;
                continue;
            }
        }
        len = ( s->curr + len > s->len ? s->len - s->curr : len);
        //len = len > s->len ? s->len : len;
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
            samples.data[i]->loop = looping[i];
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

int charger_fichier_audio(int id) {
    memcpy(playing.data[playing.len], samples.data[id], sizeof(Son));
    playing.data[playing.len]->playing = 1;
    playing.len++;
    return playing.len - 1;
}

void pause_son(int index, int pause) {
    playing.data[index]->playing = pause;
}

int is_playing(int index) {
    return playing.data[index]->playing;
}

void stop_son(int index) {
    playing.data[index]->playing = 0;
    playing.data[index]->curr = 0;
}
