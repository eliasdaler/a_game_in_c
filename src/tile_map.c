#include "tile_map.h"
#include "sdl_util.h"
#include "sprite.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <SDL_render.h>

void tile_map_load(tile_map *map, const char *path, SDL_Renderer *renderer)
{
    FILE *file = fopen(path, "r");
    if (!file) {
        fprintf(stderr, "Error loading tile map from %s: %s\n", path, strerror(errno));
        return;
    }

    char *error_reason;

    char line[255];
    if (fgets(line, 255, file)) {
        // remove newline
        line[strcspn(line, "\n")] = 0;

        map->tileset_texture = load_texture(renderer, line);
        if (!map->tileset_texture) {
            error_reason = "Can't load tileset texture";
            goto error;
        }
    } else {
        error_reason = "Can't read map tileset texture";
        goto error;
    }

    // read tilemap size
    if (fgets(line, 255, file)) {
        sscanf(line, "%d,%d", &map->size.x, &map->size.y);
    } else {
        error_reason = "Can't read map size";
        goto error;
    }

    int w = map->size.x;
    int h = map->size.y;

    // allocate tiles
    map->tiles = malloc(h * sizeof(tile*));
    for (int y = 0; y < h; ++y) {
        map->tiles[y] = malloc(w * sizeof(tile));
    }

    // read tiles
    for (int y = 0; y < h; ++y) {
        if (!fgets(line, 255, file)) {
            goto error;
        }

        // remove newline
        line[strcspn(line, "\n")] = 0;

        char *token = strtok(line, ",");
        int x = 0;
        while (token) {
            map->tiles[y][x] = atoi(token);

            ++x;
            token = strtok(NULL, ",");

            if (token && (x == w)) { // too many tiles in line!
                error_reason = "Too many tiles in line";
                goto error;
            }
        }

        if (x != w) { // not enough tiles in line
            error_reason = "Not enough tile lines";
            goto error;
        }
    }

    return;

error:
    fprintf(stderr, "Error loading tile map from %s: %s\n", path, error_reason);
    fclose(file);
}

static const int TILE_SIZE = 16;

static SDL_Rect get_tile_texture_rect(int id, int tiles_x)
{
    SDL_Rect r;
    r.x = (id % tiles_x) * TILE_SIZE;
    r.y = (id / tiles_x) * TILE_SIZE;
    r.w = TILE_SIZE;
    r.h = TILE_SIZE;
    return r;
}

void tile_map_draw(SDL_Renderer *renderer, const tile_map *map)
{
    const int w = map->size.x;
    const int h = map->size.y;

    const vec2i ts = get_sdl_texture_size(map->tileset_texture);
    const int tiles_x = ts.x / TILE_SIZE;

    sprite s = sprite_new(map->tileset_texture);
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            tile id = map->tiles[y][x];
            s.texture_rect = get_tile_texture_rect(id, tiles_x);
            s.pos = (vec2f) { x * TILE_SIZE, y * TILE_SIZE };

            sprite_draw(renderer, &s);
        }
    }
}

void tile_map_free(tile_map *map)
{
    const int h = map->size.y;
    for (int y = 0; y < h; ++y) {
        free(map->tiles[y]);
    }

    free(map->tiles);

    SDL_DestroyTexture(map->tileset_texture);
}
