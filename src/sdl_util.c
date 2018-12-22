#include "sdl_util.h"

#include <SDL_image.h>
#include <SDL_render.h>

vec2i get_sdl_texture_size(SDL_Texture *texture)
{
    vec2i result;
    SDL_QueryTexture(texture, NULL, NULL, &result.x, &result.y);
    return result;
}

SDL_Texture *load_texture(SDL_Renderer *renderer, const char *path)
{
    SDL_Surface *surface = IMG_Load(path);
    if (!surface) {
        fprintf(stderr, "Failed to load texture from %s: %s\n", path, IMG_GetError());
        return NULL;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        fprintf(stderr, "CreateTextureFromSurface failed when loading %s: %s\n", path, SDL_GetError());
    }

    SDL_FreeSurface(surface);
    return texture;
}
