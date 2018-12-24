#pragma once

struct hash_table;

struct SDL_Renderer;
struct SDL_Texture;

typedef struct resource_manager {
    struct SDL_Renderer *renderer;
    struct hash_table *textures;
    const char *texture_folder;
    const char *texture_ext;
} resource_manager;

void resource_manager_init(
    resource_manager *rm,
    struct SDL_Renderer
        *renderer); void resource_manager_free(resource_manager *rm);

struct SDL_Texture *resource_manager_load_texture(resource_manager *rm,
                                                  const char *name);
struct SDL_Texture *resource_manager_get_texture(resource_manager *rm,
                                                 const char *name);
