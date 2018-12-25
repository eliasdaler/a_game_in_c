#include "sprite.h"
#include "sdl_util.h"

#include <SDL_render.h>
#include <math.h>

sprite sprite_new(SDL_Texture *texture)
{
    sprite s = { .texture = texture };

    vec2i texture_size = get_sdl_texture_size(texture);
    s.texture_rect.w = texture_size.x;
    s.texture_rect.h = texture_size.y;

    return s;
}

void sprite_draw(const sprite *sprite, struct SDL_Renderer *renderer)
{
    SDL_Rect dest_rect;
    dest_rect.x = (sprite->position.x);
    dest_rect.y = floor(sprite->position.y);
    dest_rect.w = sprite->texture_rect.w;
    dest_rect.h = sprite->texture_rect.h;

    SDL_RenderCopy(renderer, sprite->texture, &sprite->texture_rect, &dest_rect);
}

