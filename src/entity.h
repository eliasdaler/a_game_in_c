#pragma once

#include "sprite.h"
#include "vec2.h"

#include <limits.h>

struct SDL_Renderer;

typedef int entity_id;
#define NULL_ENTITY_ID INT_MAX

typedef struct entity {
    vec2f position;
    sprite sprite;
    entity_id id;
    vec2f velocity;
    vec2f max_speed;
} entity;

void entity_init(entity *e);
void entity_set_position(entity *e, vec2f position);

void entity_update(entity *e, float dt);
void entity_draw(const entity *e, struct SDL_Renderer *renderer);
