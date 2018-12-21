#include "SDL.h"
#include "SDL_image.h"

#include <stdbool.h>
#include <math.h>

typedef struct vec2f {
    float x;
    float y;
} vec2f;

typedef struct vec2i {
    int x;
    int y;
} vec2i;

typedef struct sprite {
    vec2f pos;
    SDL_Texture *texture;
    SDL_Rect texture_rect;
} sprite;

vec2i get_sdl_texture_size(SDL_Texture* texture)
{
    vec2i result;
    SDL_QueryTexture(texture, NULL, NULL, &result.x, &result.y);
    return result;
}

sprite sprite_new(SDL_Texture *texture)
{
    sprite s = { .texture = texture };

    vec2i texture_size = get_sdl_texture_size(texture);
    s.texture_rect.w = texture_size.x;
    s.texture_rect.h = texture_size.y;

    return s;
}

void draw_sprite(SDL_Renderer *renderer, const sprite *sprite)
{
    SDL_Rect dest_rect;
    dest_rect.x = floor(sprite->pos.x);
    dest_rect.y = floor(sprite->pos.y);
    dest_rect.w = sprite->texture_rect.w;
    dest_rect.h = sprite->texture_rect.h;

    SDL_RenderCopy(renderer, sprite->texture, &sprite->texture_rect, &dest_rect);
}

SDL_Texture *load_texture(SDL_Renderer *renderer, char *path)
{
    SDL_Surface *surface = IMG_Load(path);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

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
    SDL_SetRenderDrawColor(renderer, 180, 165, 106, SDL_ALPHA_OPAQUE);

    SDL_Texture *tree_texture = load_texture(renderer, "tree.png");
    SDL_Texture *pine_tree_texture = load_texture(renderer, "pine_tree.png");
    SDL_Texture *cat_texture = load_texture(renderer, "cat.png");

    sprite tree_sprite = sprite_new(tree_texture);
    tree_sprite.pos.x = 32.f;
    tree_sprite.pos.y = 64.f;

    sprite pine_tree_sprite = sprite_new(pine_tree_texture);
    pine_tree_sprite.pos.x = 64.f;
    pine_tree_sprite.pos.y = 18.f;

    sprite cat_sprite = sprite_new(cat_texture);
    cat_sprite.texture_rect = (SDL_Rect) { 0, 0, 18, 18 };
    cat_sprite.pos.x = 64.f;
    cat_sprite.pos.y = 90.f;

    _Bool is_running = true;
    SDL_Event event;

    while (is_running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                is_running = false;
            }
        }

        SDL_RenderClear(renderer);

        draw_sprite(renderer, &tree_sprite);
        draw_sprite(renderer, &pine_tree_sprite);
        draw_sprite(renderer, &cat_sprite);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(tree_texture);
    SDL_DestroyTexture(pine_tree_texture);
    SDL_DestroyTexture(cat_texture);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    IMG_Quit();
    SDL_Quit();

    return 0;
}
