#include <SDL.h>

void drawBG(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 39, 40, 56);
    SDL_Rect bg = {0, 0, 1080, 720}; //x y sx sy
    SDL_RenderFillRect(renderer, &bg);

    SDL_SetRenderDrawColor(renderer, 19, 48, 45);
    SDL_Rect bar = {0, 0, 1080, 50};
    SDL_RenderFillRect(renderer, &bar);
}

int main(int argc, char *argv[]) {
    
}