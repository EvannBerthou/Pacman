#include <stdlib.h>
#include "audio.h"

static void *merr(void *p) {
    if (p == NULL) {
        fprintf(stderr, "Erreur malloc dans audio.c\n");
        exit(1);
    }
    return p;
}

// Le code lié à l'audio est directement pris depuis la doc de la SDL1
static SDL_AudioSpec wav_spec; // Caractéristique du fichier audio
static Sons playing = {.data = NULL, .len = 0, .max_size = 10};
static Sons samples;
static int _volume = SDL_MIX_MAXVOLUME;

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
        len = ( s->curr + len > s->len ? s->len - s->curr : len);
        //len = len > s->len ? s->len : len;
        SDL_MixAudio(stream, s->buff + s->curr, len, _volume);
        s->curr += len;
    }
}

void init_sons() {
    playing.data = merr(malloc(sizeof(Son*) * playing.max_size));
    for (int i = 0; i < playing.max_size; i++) {
        playing.data[i] = merr(malloc(sizeof(Son)));
    }

    samples.data = merr(malloc(sizeof(Son*) * SAMPLES_COUNT));
    for (int i = 0; i < SAMPLES_COUNT; i++) {
        samples.data[i] = merr(calloc(1, sizeof(Son)));
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

static void resize_playing() {
    playing.max_size *= 2;
    playing.data = merr(realloc(playing.data, sizeof(Son*) * playing.max_size));
}

int charger_fichier_audio(int id) {
    if (playing.len == playing.max_size) {
        resize_playing();
    }
    memcpy(playing.data[playing.len], samples.data[id], sizeof(Son));
    playing.data[playing.len]->playing = 1;
    playing.len++;
    return playing.len - 1;
}

void pause_son(int index, int pause) {
    playing.data[index]->playing = !pause;
}

int is_playing(int index) {
    return playing.data[index]->playing;
}

void stop_son(int index) {
    playing.data[index]->playing = 0;
    playing.data[index]->curr = 0;
}

void toggle_volume(int touche) {
    if (touche == SDLK_m) {
        _volume = (_volume == 0) ? SDL_MIX_MAXVOLUME : 0; // Echange le volume entre volume max et coupé
    }
}

int volume() {
    return _volume;
}