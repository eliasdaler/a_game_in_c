#pragma once

#include "sprite.h"
#include "vec2.h"

struct SDL_Renderer;

typedef struct entity {
    vec2f position;
    sprite sprite;
} entity;

void entity_set_position(entity *e, vec2f position);
void entity_draw(struct SDL_Renderer *renderer, const entity *e);
