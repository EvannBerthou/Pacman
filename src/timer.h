#ifndef TIMER_H
#define TIMER_H

#include <SDL/SDL.h> // Pour le Uint32

typedef struct {
    Uint32 prev_ticks;
	float duree_frame;
	float dt;

	Uint32 fps_lasttime;
	int fps;
} Timer;

Timer nouveau_timer();
void tick_timer(Timer *timer);
void print_fps(Timer *timer);

#endif
