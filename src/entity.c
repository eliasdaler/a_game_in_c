#include "entity.h"

void entity_set_position(entity* e, vec2f position)
{
    e->position = position;
    e->sprite.position = position;
}

void entity_draw(struct SDL_Renderer *renderer, const entity *e)
{
    sprite_draw(renderer, &e->sprite);
}
