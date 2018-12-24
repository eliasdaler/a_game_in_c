#include "sdl_util.h"
#include "entity.h"
#include "entity_factory.h"
#include "resource_manager.h"
#include "sprite.h"
#include "tile_map.h"

#include <SDL.h>
#include <SDL_image.h>

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

    entity_factory entity_factory;
    entity_factory_init(&entity_factory, &resource_manager);

    entity tree = entity_factory_create_entity(&entity_factory, "tree");
    entity_set_position(&tree, (vec2f) { 32.f, 64.f } );

    entity pine_tree = entity_factory_create_entity(&entity_factory, "pine_tree");
    entity_set_position(&pine_tree, (vec2f) { 90.f, 36.f } );

    entity cat = entity_factory_create_entity(&entity_factory, "cat");
    entity_set_position(&cat, (vec2f) { 70.f, 90.f } );

    tile_map map;
    tile_map_load(&map, "res/tile_maps/tile_map.txt", &resource_manager);

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
        entity_draw(renderer, &tree);
        entity_draw(renderer, &pine_tree);
        entity_draw(renderer, &cat);

        SDL_RenderPresent(renderer);
    }

    tile_map_free(&map);

    resource_manager_free(&resource_manager);
    entity_factory_free(&entity_factory);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    IMG_Quit();
    SDL_Quit();

    return 0;
}
