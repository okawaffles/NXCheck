#include <SDL.h>
#include "touch.h"

void drawTouch(SDL_Renderer *renderer, int tX, int tY) {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_Rect bg = {0, 0, 1280, 720};
    SDL_RenderFillRect(renderer, &bg);

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderDrawLine(renderer, 0, tY, 1280, tY);
	SDL_RenderDrawLine(renderer, tX, 0, tX, 720);
}

int touchWithin(int chX, int chY, int tX, int tY, int bX, int bY) {
	if (chX >= tX && chX <= bX && chY >= tY && chY <= bY)
    {
        return 1;
    } else return 0;
}