#pragma once

#include "vec2.h"

#include <stdbool.h>
#include <stddef.h>

struct SDL_Texture;
struct SDL_Renderer;

struct resource_manager;
struct entity_manager;

typedef int tile;

typedef struct tile_map {
    tile **tiles;  // 2d array of tiles
    vec2i size;
    struct SDL_Texture *tileset_texture;
} tile_map;

bool tile_map_load(tile_map *map, const char *path, struct entity_manager *em,
                    struct resource_manager *rm);

void tile_map_draw(const tile_map *map, struct SDL_Renderer *renderer);
void tile_map_free(tile_map *map);
