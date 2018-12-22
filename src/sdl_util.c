#include "sdl_util.h"

#include <SDL_image.h>
#include <SDL_render.h>

vec2i get_sdl_texture_size(SDL_Texture* texture)
{
    vec2i result;
    SDL_QueryTexture(texture, NULL, NULL, &result.x, &result.y);
    return result;
}

SDL_Texture *load_texture(SDL_Renderer *renderer, char *path)
{
    SDL_Surface *surface = IMG_Load(path);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}
