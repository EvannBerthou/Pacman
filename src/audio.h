#ifndef AUDIO_H
#define AUDIO_H

#include <SDL/SDL.h>

// Structure qui stocke les caractéristique d'un fichier audio
typedef struct {
    Uint8 *buff; // L'audio
    Uint32 len;  // La taille de l'audio
    Uint32 curr; // L'avancé de la lecture
    int playing; // En train d'être joué ?
    int loop;    // Doit-il bouclé ?
} Son;

// Structure qui stocke une liste de fichier audios
typedef struct {
    Son **data; // Liste de pointeurs vers des Son
    int len; // Taille de la liste
    int max_size; // Nombre max d'audios dans la liste
} Sons;

// Chargement
void init_sons();
int charger_fichier_audio(int id);

// Outils
void pause_son(int index, int pause);
int is_playing(int index);
void stop_son(int index);
void toggle_volume(int touche);
int volume();
int audio_actif();

#endif
