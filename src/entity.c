#include "entity.h"

void entity_init(entity *e)
{
    e->position  = (vec2f) { 0.f, 0.f };
    e->id = NULL_ENTITY_ID;
    e->velocity  = (vec2f) { 0.f, 0.f };
    e->max_speed = (vec2f) { 0.f, 0.f };
}

void entity_set_position(entity* e, vec2f position)
{
    e->position = position;
    e->sprite.position = position;
}

void entity_update(entity *e, float dt)
{
    vec2f new_position = e->position;
    new_position.x += e->velocity.x * dt;
    new_position.y += e->velocity.y * dt;

    entity_set_position(e, new_position);
}

void entity_draw(const entity *e, struct SDL_Renderer *renderer)
{
    sprite_draw(&e->sprite, renderer);
}
