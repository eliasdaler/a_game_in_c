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

void draw_sprite(SDL_Renderer *renderer, const sprite *sprite)
{
    SDL_Rect dest_rect;
    dest_rect.x = floor(sprite->pos.x);
    dest_rect.y = floor(sprite->pos.y);
    dest_rect.w = sprite->texture_rect.w;
    dest_rect.h = sprite->texture_rect.h;

    SDL_RenderCopy(renderer, sprite->texture, &sprite->texture_rect, &dest_rect);
}

