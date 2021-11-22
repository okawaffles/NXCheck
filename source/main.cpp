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
    SDL_Event event;
    SDL_Window *window;
    SDL_Renderer *renderer;
    int done = 0;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) { //mandatory on switch apparently
        SDL_Log("SDL_Init: %s\n", SDL_GetError());
        return -1;
    }

    window = SDL_CreateWindow("sdl2_gles2", 0, 0, 1920, 1080, 0); //create a handheld window, remember to add on docked support.
    if (!window) {
        SDL_Log("SDL_CreateWindow: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    //create renderer
    renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        SDL_Log("SDL_CreateRenderer: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    // from switch-examples/graphics/sdl2/sdl2-simple/main.cpp
    //
    // open CONTROLLER_PLAYER_1 and CONTROLLER_PLAYER_2 
    // when railed, both joycons are mapped to joystick #0,
    // else joycons are individually mapped to joystick #0, joystick #1, ...
    // https://github.com/devkitPro/SDL/blob/switch-sdl2/src/joystick/switch/SDL_sysjoystick.c#L45
    for (int i = 0; i < 2; i++) {
        if (SDL_JoystickOpen(i) == NULL) {
            SDL_Log("SDL_JoystickOpen: %s\n", SDL_GetError());
            SDL_Quit();
            return -1;
        }
    }

    SDL_SetWindowSize(window, 1280, 720); //remove later when docked support is added.

    while (!done) { //also taken, but revised
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_JOYAXISMOTION:
                    SDL_Log("Joystick %d axis %d value: %d\n",
                            event.jaxis.which,
                            event.jaxis.axis, event.jaxis.value);
                    break;

                case SDL_JOYBUTTONDOWN:
                    SDL_Log("Joystick %d button %d down\n",
                            event.jbutton.which, event.jbutton.button);
                    // https://github.com/devkitPro/SDL/blob/switch-sdl2/src/joystick/switch/SDL_sysjoystick.c#L52
                    // seek for joystick #0
                    if (event.jbutton.which == 0) {
                        if (event.jbutton.button == 10) {
                            // (+) button down
                            done = 1;
                        }
                    }
                    break;

                default:
                    break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        drawBG(renderer);    

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}