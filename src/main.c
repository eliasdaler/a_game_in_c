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
        return EXIT_FAILURE;
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
        return EXIT_FAILURE;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_RenderSetScale(renderer, 3.0f, 3.0f);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

    resource_manager resource_manager;
    resource_manager_init(&resource_manager, renderer);

    entity_manager entity_manager;
    entity_manager_init(&entity_manager, &resource_manager);

    tile_map map;
    if (!tile_map_load(&map, "res/tile_maps/tile_map.txt", &entity_manager, &resource_manager)) {
        return EXIT_FAILURE;
    }

    entity* player = entity_manager_get_entity_by_tag(&entity_manager, "player");
    player->velocity.x = 30.f;
    assert(player);

    bool is_running = true;
    SDL_Event event;

    // Fix your timestep! game loop
    const float dt = 1 / 60.f; // 60 fps
    float accumulator = 0.f;
    float frame_time = 0.f;

    uint32_t current_time = SDL_GetTicks();

    while (is_running) {
        uint32_t new_time = SDL_GetTicks();
        frame_time = (new_time - current_time) / 1000.f; // time in SDL is stored in ms
        current_time = new_time;

        accumulator += frame_time;

        // input/events
        while (accumulator >= dt) {
            // handle events
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    is_running = false;
                }
            }

            // handle input
            const Uint8* curr_key_states = SDL_GetKeyboardState( NULL );
            vec2f walk_vec = { 0.f, 0.f };
            if (curr_key_states[SDL_SCANCODE_UP]) {
                walk_vec.y -= 1.f;
            }

            if (curr_key_states[SDL_SCANCODE_DOWN]) {
                walk_vec.y += 1.f;
            }

            if (curr_key_states[SDL_SCANCODE_LEFT]) {
                walk_vec.x -= 1.f;
            }

            if (curr_key_states[SDL_SCANCODE_RIGHT]) {
                walk_vec.x += 1.f;
            }

            player->velocity.x = walk_vec.x * player->max_speed.x;
            player->velocity.y = walk_vec.y * player->max_speed.y;

            // update
            for (entity_id id = 0; id < entity_manager.last_id; ++id) {
                entity *e = entity_manager_get_entity(&entity_manager, id);
                if (e) {
                    entity_update(e, dt);
                }
            }

            accumulator -= dt;
        }

        // render
        SDL_RenderClear(renderer);

        tile_map_draw(&map, renderer);

        for(entity_id id = 0; id < entity_manager.last_id; ++id) {
            entity *e = entity_manager_get_entity(&entity_manager, id);
            if (e) {
                entity_draw(e, renderer);
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

    return EXIT_SUCCESS;
}
