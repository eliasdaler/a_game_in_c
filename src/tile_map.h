#pragma once

#include "vec2.h"

#include <stddef.h>

struct SDL_Texture;
struct SDL_Renderer;

typedef int tile;

typedef struct tile_map {
    tile **tiles;  // 2d array of tiles
    vec2i size;
    struct SDL_Texture *tileset_texture;
} tile_map;

void tile_map_load(tile_map *map, const char *path,
                   struct SDL_Renderer *renderer);

void tile_map_draw(struct SDL_Renderer *renderer, const tile_map *map);
void tile_map_free(tile_map *map);
