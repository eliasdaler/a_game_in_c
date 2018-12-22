#pragma once

#include "vec2.h"

struct SDL_Texture;
struct SDL_Renderer;

vec2i get_sdl_texture_size(struct SDL_Texture *texture);
struct SDL_Texture *load_texture(struct SDL_Renderer *renderer,
                                 const char *path);
