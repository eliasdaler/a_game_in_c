#pragma once

#include "vec2.h"

#include <SDL_rect.h>
#include <SDL_render.h>

typedef struct sprite {
    vec2f pos;
    SDL_Texture *texture;
    SDL_Rect texture_rect;
} sprite;

sprite sprite_new(SDL_Texture *texture);
void draw_sprite(SDL_Renderer *renderer, const sprite *sprite);
SDL_Texture *load_texture(SDL_Renderer *renderer, char *path);
