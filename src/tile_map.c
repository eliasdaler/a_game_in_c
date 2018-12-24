#include "tile_map.h"

#include "hash_table.h"
#include "entity_manager.h"
#include "resource_manager.h"

#include "sdl_util.h"
#include "sprite.h"
#include "string_util.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <SDL_render.h>

static _Bool read_line(FILE *file, char *line, int buffer_size, int *line_number)
{
    _Bool ok = fgets(line, buffer_size, file);
    if (ok) {
        ++(*line_number);
        remove_ending_newline(line);
    }
    return ok;
}

static char* create_error_string_not_maching_format(const char *line, const char *fmt)
{
    size_t str_size = strlen(fmt) + strlen(line) - 2; // 2 because of "%s"
    char *str = malloc(str_size + 1);
    snprintf(str, str_size, fmt, line);
    return str;
}

static hash_table *read_line_kv(const char *str)
{
    hash_table *ht = strkey_hash_table_new();

    char key_buf[64];
    char value_buf[64];

    char *str_copy = copy_string(str);
    char *token = strtok(str_copy, ",");
    while (token) {
        sscanf(token, "%[^=]=%s", key_buf, value_buf);
        hash_table_set(ht, copy_string(key_buf), copy_string(value_buf));
        token = strtok(NULL, ",");
    }
    free(str_copy);

    return ht;
}

_Bool tile_map_load(tile_map *map, const char *path,
                    struct entity_manager *em,
                    struct resource_manager *rm)
{
    FILE *file = fopen(path, "r");
    if (!file) {
        fprintf(stderr, "Error loading tile map from %s: %s\n", path, strerror(errno));
        return false;
    }

    char *error_reason = "unknown error";
    _Bool remove_error_reason_str = false; // if allocated string - remove it in the end
    int line_number = 0;

#define LINE_BUFFER_SIZE 255
    char line[LINE_BUFFER_SIZE];
    if (read_line(file, line, LINE_BUFFER_SIZE, &line_number)) {
        map->tileset_texture = resource_manager_get_texture(rm, line);
        if (!map->tileset_texture) {
            error_reason = "Can't load tileset texture";
            goto error;
        }
    } else {
        error_reason = "Can't read map tileset texture";
        goto error;
    }

    if (!read_line(file, line, LINE_BUFFER_SIZE, &line_number)) {
        error_reason = "EOF reached before finding ENTITIES_BEGIN token";
        goto error;
    }

    if (strcmp(line, "ENTITIES_BEGIN")) {
        error_reason = "Expected ENTITIES_BEGIN token after texture filename";
        goto error;
    }

    for (;;) {
        if (!read_line(file, line, LINE_BUFFER_SIZE, &line_number)) {
            error_reason = "EOF reached before finding ENTITIES_END token";
            goto error;
        }

        if(strcmp(line, "ENTITIES_END") == 0) {
            break;
        }

        hash_table* properties = read_line_kv(line);
        const char* prefab_name = hash_table_get(properties, "prefab");
        const char* pos_x_str = hash_table_get(properties, "x");
        const char* pos_y_str = hash_table_get(properties, "y");

        if(prefab_name && pos_x_str && pos_y_str) {
            vec2f pos;
            pos.x = atof(pos_x_str);
            pos.y = atof(pos_y_str);

            entity *e = entity_manager_create_entity(em, prefab_name);
            entity_set_position(e, pos);

            const char* tag = hash_table_get(properties, "tag");
            if (tag) {
                entity_manager_set_entity_tag(em, e, tag);
            }

            hash_table_free(properties);
        } else {
            static const char* fmt = "Line '%s' doesn't match format 'x=...,y=...,prefab=%s'";
            error_reason = create_error_string_not_maching_format(line, fmt);
            remove_error_reason_str = true;
            hash_table_free(properties);
            goto error;
        }
    }

    // read tilemap size
    if (read_line(file, line, LINE_BUFFER_SIZE, &line_number)) {
        if (!sscanf(line, "map_size=%d,%d", &map->size.x, &map->size.y)) {
            static const char* fmt = "Line '%s' doesn't match format 'map_size=<x>,<y>'";
            error_reason = create_error_string_not_maching_format(line, fmt);
            remove_error_reason_str = true;
            goto error;
        }
    } else {
        error_reason = "EOF reached before could read map size";
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
        if (!read_line(file, line, LINE_BUFFER_SIZE, &line_number)) {
            error_reason = "Not enough tile lines";
            goto error;
        }

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

    return true;

error:
    fprintf(stderr, "Error loading tile map from '%s', line %d: %s\n", path, line_number, error_reason);
    if (remove_error_reason_str) {
        free(error_reason);
    }
    fclose(file);
    return false;
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
            s.position = (vec2f) { x * TILE_SIZE, y * TILE_SIZE };

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
}
