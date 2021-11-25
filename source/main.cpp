#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <switch.h>
#include <unistd.h>


// taken from sdl2-demo
SDL_Texture * render_text(SDL_Renderer *renderer, const char* text, TTF_Font *font, SDL_Color color, SDL_Rect *rect) 
{
    SDL_Surface *surface;
    SDL_Texture *texture;

    surface = TTF_RenderText_Solid(font, text, color);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    rect->w = surface->w;
    rect->h = surface->h;

    SDL_FreeSurface(surface);

    return texture;
}

void easyDrawText(SDL_Renderer *renderer, TTF_Font *font, const char* text, SDL_Color color, int x, int y) {
    SDL_Texture* easyDrawObj_tex = NULL;
    SDL_Rect easyDrawObj_rect = {x, y, 0, 0};
    easyDrawObj_tex = render_text(renderer, text, font, color, &easyDrawObj_rect);
    SDL_RenderCopy(renderer, easyDrawObj_tex, NULL, &easyDrawObj_rect);
    SDL_DestroyTexture(easyDrawObj_tex);
}


void drawBG(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 53, 62, 74, 255);
    SDL_Rect bg = {0, 0, 1280, 720}; //x y sx sy
    SDL_RenderFillRect(renderer, &bg);

    SDL_SetRenderDrawColor(renderer, 55, 103, 112, 255);
    SDL_Rect bar = {0, 0, 1280, 50};
    SDL_RenderFillRect(renderer, &bar);


    //nxcheck right top text
    SDL_Texture* nxcheck_tex = NULL; 
    SDL_Color white = {255, 255, 255};
    TTF_Font* fontdef = TTF_OpenFont("mplus1.ttf", 36);
    SDL_Rect nxcheck_rect = { 2, -7, 0, 0 };
    nxcheck_tex = render_text(renderer, "NXCheck", fontdef, white, &nxcheck_rect);

    //hbmenu right bottom text
    SDL_Texture* hbmenu_tex = NULL; 
    SDL_Rect hbmenu_rect = { 0, 720 - 45, 0, 0 };
    hbmenu_tex = render_text(renderer, "Press + to return to hbmenu", fontdef, white, &hbmenu_rect);

    SDL_RenderCopy(renderer, nxcheck_tex, NULL, &nxcheck_rect);
    SDL_RenderCopy(renderer, hbmenu_tex, NULL, &hbmenu_rect);

    // destroy textures n stuff
    SDL_DestroyTexture(nxcheck_tex);
    SDL_DestroyTexture(hbmenu_tex);
    TTF_CloseFont(fontdef);
}


void drawOptions(SDL_Renderer *renderer) {
    SDL_Color white = {255, 255, 255};
    TTF_Font* fontdef = TTF_OpenFont("mplus1.ttf", 24);

    SDL_SetRenderDrawColor(renderer, 55, 103, 112, 255);
    SDL_Rect op0 = {0, 75, 250, 50};
    SDL_RenderFillRect(renderer, &op0);
    easyDrawText(renderer, fontdef, "Touchscreen", white, 5, 80);



    //clean up
    TTF_CloseFont(fontdef);
}




int main(int argc, char *argv[]) {
    romfsInit();
    chdir("romfs:/");

    SDL_Event event;
    SDL_Window *window;
    SDL_Renderer *renderer;
    int done = 0;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) { //mandatory on switch apparently
        SDL_Log("SDL_Init: %s\n", SDL_GetError());
        return -1;
    }

    TTF_Init();

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
        drawOptions(renderer);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}