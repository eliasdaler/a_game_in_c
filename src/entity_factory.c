#include "entity_factory.h"

#include "hash_table.h"
#include "string_util.h"
#include "resource_manager.h"

static void add_prefab(entity_factory *factory, char *prefab_name, entity prefab)
{
    char *key = copy_string(prefab_name);
    entity *value = malloc(sizeof(entity));
    memcpy(value, &prefab, sizeof(entity));

    hash_table_set(factory->prefabs, key, value);
}

void entity_factory_init(entity_factory *factory, resource_manager *rm)
{
    factory->prefabs = strkey_hash_table_new();

    // TODO: load prefabs from Lua
    entity tree;
    entity_init(&tree);
    struct SDL_Texture *tree_texture = resource_manager_get_texture(rm, "tree");
    tree.sprite = sprite_new(tree_texture);
    add_prefab(factory, "tree", tree);

    entity pine_tree;
    entity_init(&pine_tree);
    struct SDL_Texture *pine_tree_texture = resource_manager_get_texture(rm, "pine_tree");
    pine_tree.sprite = sprite_new(pine_tree_texture);
    add_prefab(factory, "pine_tree", pine_tree);

    entity cat;
    entity_init(&cat);
    struct SDL_Texture *cat_texture = resource_manager_get_texture(rm, "cat");
    cat.sprite = sprite_new(cat_texture);
    cat.sprite.texture_rect = (SDL_Rect) { 0, 0, 18, 18 };
    cat.max_speed = (vec2f) { 30.f, 30.f };
    add_prefab(factory, "cat", cat);
}

void entity_factory_free(entity_factory *factory)
{
    hash_table_free(factory->prefabs);
}

entity entity_factory_create_entity(entity_factory *factory,
                                    const char *prefab_name)
{
    const entity* prefab_entity = hash_table_get(factory->prefabs, prefab_name);
    return *prefab_entity; // make a copy
}
