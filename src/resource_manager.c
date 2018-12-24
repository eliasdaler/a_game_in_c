#include "resource_manager.h"

#include "hash_table.h"
#include "sdl_util.h"

#include <SDL_render.h>

static void texture_deleter(void* texture)
{
    (void)texture; // actually SDL_renderer will do it for us, so do nothing here
}

void resource_manager_init(resource_manager *rm, struct SDL_Renderer *renderer)
{
    rm->renderer = renderer;

    rm->textures = strkey_hash_table_new();
    rm->textures->value_deleter_func = texture_deleter;

    rm->texture_folder = "res/images/";
    rm->texture_ext = ".png";
}

void resource_manager_free(resource_manager *rm)
{
    hash_table_free(rm->textures);
}

static char* get_texture_path(const resource_manager *rm, const char *name)
{
    char* path = malloc(strlen(rm->texture_folder) + strlen(name) + strlen(rm->texture_ext) + 1);
    strcpy(path, rm->texture_folder);
    strcat(path, name);
    strcat(path, rm->texture_ext);
    return path;
}

struct SDL_Texture* resource_manager_load_texture(resource_manager *rm, const char *name)
{
    char *texture_path = get_texture_path(rm, name);
    SDL_Texture *texture = load_texture(rm->renderer, texture_path);
    if (texture) {
        hash_table_set(rm->textures, texture_path, texture);
    } else {
        fprintf(stderr, "Resource manager: failed to load %s", texture_path);
        free(texture_path);
    }
    return texture;
}

struct SDL_Texture *resource_manager_get_texture(resource_manager *rm, const char *name)
{
    SDL_Texture* texture = hash_table_get(rm->textures, name);
    if (!texture) { // wasn't loaded before
        texture = resource_manager_load_texture(rm, name);
    }
    return texture;
}
