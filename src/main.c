#include "sdl_util.h"
#include "entity.h"
#include "entity_manager.h"
#include "resource_manager.h"
#include "sprite.h"
#include "tile_map.h"

#include <SDL.h>
#include <SDL_image.h>

#include <assert.h>
#include <stdbool.h>

#include "hash_table_test.h"

int main(int argc, char *argv[])
{
    hash_table_test();

    SDL_Init(SDL_INIT_VIDEO);

    // init SDL_image
    int imgFlags = IMG_INIT_PNG;
    if(!(IMG_Init(imgFlags) & imgFlags)) {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "Game",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        480,
        432,
        SDL_WINDOW_SHOWN);

    if (!window) {
        printf("Couldn't create window! SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_RenderSetScale(renderer, 3.0f, 3.0f);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

    resource_manager resource_manager;
    resource_manager_init(&resource_manager, renderer);

    entity_manager entity_manager;
    entity_manager_init(&entity_manager, &resource_manager);

    tile_map map;
    if (!tile_map_load(&map, "res/tile_maps/tile_map.txt", &entity_manager, &resource_manager)) {
        return 1;
    }

    entity* player = entity_manager_get_entity_by_tag(&entity_manager, "player");
    assert(player);

    _Bool is_running = true;
    SDL_Event event;

    while (is_running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                is_running = false;
            }
        }

        SDL_RenderClear(renderer);

        tile_map_draw(renderer, &map);

        // for now we'll assume that entities mostly lay close to each other
        for(entity_id id = 0; id < entity_manager.last_id; ++id) {
            entity *e = entity_manager_get_entity(&entity_manager, id);
            if (e) {
                entity_draw(renderer, e);
            }
        }

        SDL_RenderPresent(renderer);
    }

    tile_map_free(&map);

    entity_manager_free(&entity_manager);
    resource_manager_free(&resource_manager);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    IMG_Quit();
    SDL_Quit();

    return 0;
}
