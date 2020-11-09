#include "timer.h"

Timer nouveau_timer() {
	Timer t = {
		.prev_ticks = SDL_GetTicks(),
		.duree_frame = 1000.f / 60.f,
		.dt = 0,
		.fps_lasttime = SDL_GetTicks(),
		.fps = 0,
	};
	return t;
}

// Calcul le temps qu'a mit une frame et attend le temps restant pour rester à 60 fps (16.666ms)
void tick_timer(Timer *timer) {
	Uint32 curr_ticks = SDL_GetTicks();
	float dt = (float)(curr_ticks - timer->prev_ticks);
	if (dt < timer->duree_frame)
		SDL_Delay(timer->duree_frame - dt);
	else
		timer->fps = 1000.f / dt;
	timer->prev_ticks = curr_ticks;
	timer->dt = dt / 1000.f;
}

void print_fps(Timer *timer) {
	// Affiche les fps qu'une fois toutes les secondes
	if (timer->prev_ticks > 1000 && timer->fps_lasttime < timer->prev_ticks - 1000) {
		timer->fps_lasttime = timer->prev_ticks;
		printf("FPS : %d\n", timer->fps);
	}
}
