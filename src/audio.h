#ifndef AUDIO_H
#define AUDIO_H

#include <SDL/SDL.h>

typedef struct {
    Uint8 *buff;
    Uint32 len;
    Uint32 curr;
} Son;

Son *charger_fichier_audio(const char *fichier);
int jouer_fichier_audio();

#endif
