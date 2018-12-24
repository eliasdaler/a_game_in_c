#include "entity_manager.h"

#include "hash_table.h"
#include "entity_factory.h"

static entity_id* alloc_new_id(entity_manager *em)
{
    entity_id* id = malloc(sizeof(int));
    *id = em->last_id;
    ++em->last_id;
    return id;
}

void entity_manager_init(entity_manager *em, struct resource_manager *rm)
{
    em->entities = hash_table_new(sizeof(entity_id));
    em->last_id = 0;
    entity_factory_init(&em->factory, rm);
}

void entity_manager_free(entity_manager *em)
{
    hash_table_free(em->entities);
    entity_factory_free(&em->factory);
}

struct entity *entity_manager_create_entity(entity_manager *em, const char *prefab_name)
{
    entity *e = malloc(sizeof(entity));
    *e = entity_factory_create_entity(&em->factory, prefab_name);
    entity_id *id = alloc_new_id(em);

    hash_table_set(em->entities, id, e);

    return e;
}

struct entity *entity_manager_get_entity(const entity_manager *em, entity_id id)
{
    return hash_table_get(em->entities, &id);
}
