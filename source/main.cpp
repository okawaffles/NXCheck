#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <switch.h>
#include <unistd.h>

#include "touch.h"

int mode = 0;


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
    TTF_Font* fontdef = TTF_OpenFont("mplus1.ttf", 30);

    SDL_SetRenderDrawColor(renderer, 55, 103, 112, 255);
    SDL_Rect op0 = {0, 75, 250, 60};
    SDL_RenderFillRect(renderer, &op0);
    easyDrawText(renderer, fontdef, "Touchscreen", white, 5, 80);

    SDL_SetRenderDrawColor(renderer, 55, 103, 112, 255);
    SDL_Rect op1 = {0, 140, 250, 60};
    SDL_RenderFillRect(renderer, &op1);
    easyDrawText(renderer, fontdef, "Buttons", white, 5, 145);



    //clean up
    TTF_CloseFont(fontdef);
}




int main(int argc, char *argv[]) {
    romfsInit();
    chdir("romfs:/");

    SDL_Window *window;
    SDL_Renderer *renderer;

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

    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    PadState pad;
    padInitializeDefault(&pad);
    hidInitializeTouchScreen();


    SDL_SetWindowSize(window, 1280, 720); //remove later if docked support is added (most likely not).

    while (appletMainLoop()) {
        padUpdate(&pad);
        u64 kDown = padGetButtonsDown(&pad);

        if (kDown & HidNpadButton_Plus)
            break; // break in order to return to hbmenu


        //touchscreen stuff
        int tX = -1;
        int tY = -1;
        HidTouchScreenState state={0};
        hidGetTouchScreenStates(&state, 1);

        for(s32 i=0; i<state.count; i++)
        {
            tX = state.touches[i].x;
            tY = state.touches[i].y;
        }

        if(touchWithin(tX, tY, 0, 75, 250, 135) || kDown & HidNpadButton_A) {
            mode = 1;
        }


        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);


        switch(mode) {
            case 0:
                drawBG(renderer);
                drawOptions(renderer);
                break;
            case 1:
                drawTouch(renderer, tX, tY);
                SDL_Color darkgray = { 50, 50, 50, 255 };
                TTF_Font* fontdef = TTF_OpenFont("mplus1.ttf", 36);
                easyDrawText(renderer, fontdef, "(Press B to return)", darkgray, 5, 720 - 50);
                TTF_CloseFont(fontdef);
                if(kDown & HidNpadButton_B) {mode = 0;};
                break;
        }


        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}