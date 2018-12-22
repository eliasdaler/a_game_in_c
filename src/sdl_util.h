#pragma once

#include "vec2.h"

#include <SDL_render.h>

vec2i get_sdl_texture_size(SDL_Texture *texture);
SDL_Texture *load_texture(SDL_Renderer *renderer, char *path);
