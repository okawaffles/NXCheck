#ifndef TOUCH_H_INCLUDED
#define TOUCH_H_INCLUDED

void drawTouch(SDL_Renderer *renderer, int tX, int tY);
int touchWithin(int chX, int chY, int tX, int tY, int bX, int bY);

#endif