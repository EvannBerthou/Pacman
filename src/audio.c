#include <stdlib.h>
#include "audio.h"

// Le code lié à l'audio est directement pris depuis la doc de la SDL1
static SDL_AudioSpec wav_spec; // Caractéristique du fichier audio
int a = 0;

// Permet d'avancer la lecture du fichier audio
// Provient de la doc SDL1
static void retour_audio(void *userdata, Uint8 *stream, int len) {
    Son **sons = (Son**) userdata;

    for (int i = 0; i < a; i++) {
        Son *s = sons[i];
        if (s->len == 0)
            return;
        len = ( len > s->len ? s->len : len);
        SDL_MixAudio(stream, s->buff + s->curr, len, SDL_MIX_MAXVOLUME);
        s->curr += len;
        s->len -= len;
    }
}

Son *charger_fichier_audio(const char *fichier) {
    // Charge le fichier musique
    static Son *sons[10] = {NULL};
    static int sons_index = 0;
    Son *son = malloc(sizeof(Son));
    if (SDL_LoadWAV(fichier, &wav_spec, &son->buff, &son->len) != NULL ){
        sons[sons_index] = son;
        sons_index++;
        a++;
        wav_spec.callback = retour_audio;
        wav_spec.userdata = sons;
        return son;
    }
    fprintf(stderr, "erreur chargement %s\n", fichier);
    free(son);
    return NULL;


}

int jouer_fichier_audio() {
    // Ouvre le périphérique audio
    if (SDL_OpenAudio(&wav_spec, NULL) < 0 ) {
    #ifdef DEBUG
      fprintf(stderr, "Erreur lors de l'ouverture de l'audio: %s\n", SDL_GetError());
    #endif
      return -1;
    }
    SDL_PauseAudio(0);
    return 0;
}

