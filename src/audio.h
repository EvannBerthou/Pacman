#ifndef AUDIO_H
#define AUDIO_H

#include <SDL/SDL.h>

typedef struct {
    Uint8 *buff;
    Uint32 len;
    Uint32 curr;
    int playing;
    int loop;
} Son;

typedef struct {
    Son **data;
    int len;
} Sons;

void init_sons();
void charger_fichier_audio(int id);

#endif
