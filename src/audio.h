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
    int max_size;
} Sons;

void init_sons();
int charger_fichier_audio(int id);
void pause_son(int index, int pause);
int is_playing(int index);
void stop_son(int index);
void toggle_volume(int touche);
int volume();
int duree_son(int index);
int audio_actif();

#endif
