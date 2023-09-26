#include "neural_network.h"
#include "visual.h"

int VisualizerInit(Visualizer* this, int width, int height, const char* fontPath, int fontSize) {

    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        fprintf(stderr, "Failed to initialize SDL: %s\n", SDL_GetError());
        return -1;
    };
    if (TTF_Init()) {
        fprintf(stderr, "Failed to initialize SDL_ttf: %s\n", SDL_GetError());
        return -1;
    }

    if (SDL_CreateWindowAndRenderer(width, height, 0, &this->window, &this->renderer)) {
        fprintf(stderr, "Failed to create window or renderer: %s\n", SDL_GetError());
        return -1;
    }

    if (!(this->font = TTF_OpenFont(fontPath, fontSize))) {
        fprintf(stderr, "Failed to open font '%s': %s", fontPath, SDL_GetError());
        return -1;
    }

    this->surfaceCnt = 0;
    this->surface = malloc(sizeof(SDL_Surface*) * MAX_SURFACE_CNT);
    this->textureCnt = 0;
    this->texture = malloc(sizeof(SDL_Texture*) * MAX_TEXTURE_CNT);
}

void VisualizerFree(Visualizer* this) {
    if (!this)
        return;
    for (int i = 0; i < this->surfaceCnt; i++)
        SDL_FreeSurface(this->surface[i]);
    for (int i = 0; i < this->textureCnt; i++)
        SDL_DestroyTexture(this->texture[i]);
    free(this->surface);
    free(this->texture);
    TTF_CloseFont(this->font);
    SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->window);
}

void VisualizerDrawMeta(Visualizer* this) {
    int width, height;
    char dimension[16];
    SDL_GetWindowSize(this->window, &width, &height);
    sprintf(dimension, "(%d, %d)", width, height);
    VisualizerDrawText(this, dimension, 10, 10, (SDL_Color){255,255,255});
}

void VisualizerPresent(Visualizer* this) {
    VisualizerDrawMeta(this);
    SDL_RenderPresent(this->renderer);
    for (int i = 0; i < this->surfaceCnt; i++)
        SDL_FreeSurface(this->surface[i]);
    for (int i = 0; i < this->textureCnt; i++)
        SDL_DestroyTexture(this->texture[i]);
    this->surfaceCnt = 0;
    this->textureCnt = 0;
}

void VisualizerClear(Visualizer* this) {
    SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(this->renderer);
}

void VisualizerDrawText(Visualizer* this, const char* text, int x, int y, SDL_Color color) {
    if (
        this->surfaceCnt >= MAX_SURFACE_CNT || 
        this->textureCnt >= MAX_TEXTURE_CNT
    ) {
        fprintf(stderr, "Surface or texture limit exceed");
        return;
    }
    SDL_Surface* currSurface =
        this->surface[this->surfaceCnt++] = TTF_RenderText_Solid(this->font, text, color);
    SDL_Texture* currTexture =
        this->texture[this->textureCnt++] = SDL_CreateTextureFromSurface(this->renderer, currSurface);
    SDL_Rect src = {0, 0, currSurface->w, currSurface->h};
    SDL_Rect dest = {x, y, currSurface->w, currSurface->h};
    SDL_RenderCopy(this->renderer, currTexture, &src, &dest);
}
