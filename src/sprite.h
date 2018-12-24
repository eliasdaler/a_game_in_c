#pragma once

#include "vec2.h"

#include <SDL_rect.h>

struct SDL_Texture;
struct SDL_Renderer;

typedef struct sprite {
    vec2f position;
    struct SDL_Texture *texture;
    struct SDL_Rect texture_rect;
} sprite;

sprite sprite_new(struct SDL_Texture *texture);

void sprite_draw(struct SDL_Renderer *renderer, const sprite *sprite);
