#include "sdl_util.h"
#include "sprite.h"
#include "tile_map.h"

#include <SDL.h>
#include <SDL_image.h>

#include <stdbool.h>


int main(int argc, char *argv[])
{
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

    SDL_Texture *tree_texture = load_texture(renderer, "res/images/tree.png");
    SDL_Texture *pine_tree_texture = load_texture(renderer, "res/images/pine_tree.png");
    SDL_Texture *cat_texture = load_texture(renderer, "res/images/cat.png");

    sprite tree_sprite = sprite_new(tree_texture);
    tree_sprite.pos.x = 32.f;
    tree_sprite.pos.y = 64.f;

    sprite pine_tree_sprite = sprite_new(pine_tree_texture);
    pine_tree_sprite.pos.x = 90.f;
    pine_tree_sprite.pos.y = 36.f;

    sprite cat_sprite = sprite_new(cat_texture);
    cat_sprite.texture_rect = (SDL_Rect) { 0, 0, 18, 18 };
    cat_sprite.pos.x = 70.f;
    cat_sprite.pos.y = 90.f;

    tile_map map;
    tile_map_load(&map, "res/tile_maps/tile_map.txt", renderer);

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
        sprite_draw(renderer, &tree_sprite);
        sprite_draw(renderer, &pine_tree_sprite);
        sprite_draw(renderer, &cat_sprite);

        SDL_RenderPresent(renderer);
    }

    tile_map_free(&map);

    SDL_DestroyTexture(tree_texture);
    SDL_DestroyTexture(pine_tree_texture);
    SDL_DestroyTexture(cat_texture);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    IMG_Quit();
    SDL_Quit();

    return 0;
}
