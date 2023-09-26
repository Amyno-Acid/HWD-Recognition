#include <stdio.h>
#include "SDL.h"
#include "SDL_ttf.h"

#ifndef VISUAL_H
#define VISUAL_H

#define MAX_SURFACE_CNT 512
#define MAX_TEXTURE_CNT 512

typedef struct Visualizer {
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    int surfaceCnt;
    SDL_Surface** surface;
    int textureCnt;
    SDL_Texture** texture;
} Visualizer;

int VisualizerInit(Visualizer* this, int width, int height, const char* fontPath, int fontSize);
void VisualizerFree(Visualizer* this);
void VisualizerDrawMeta(Visualizer* this);
void VisualizerPresent(Visualizer* this);
void VisualizerClear(Visualizer* this);
void VisualizerDrawText(Visualizer* this, const char* text, int x, int y, SDL_Color color);

#endif
