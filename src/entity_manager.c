#include "entity_manager.h"

#include "hash_table.h"
#include "entity_factory.h"
#include "string_util.h"

static entity_id* copy_entity_id(entity_id id)
{
    entity_id* copy = malloc(sizeof(entity_id));
    *copy = id;
    return copy;
}

void entity_manager_init(entity_manager *em, struct resource_manager *rm)
{
    em->entities = hash_table_new(sizeof(entity_id));
    em->last_id = 0;
    entity_factory_init(&em->factory, rm);
    em->tagged_entities = strkey_hash_table_new();
}

void entity_manager_free(entity_manager *em)
{
    hash_table_free(em->entities);
    hash_table_free(em->tagged_entities);
    entity_factory_free(&em->factory);
}

entity *entity_manager_create_entity(entity_manager *em, const char *prefab_name)
{
    entity *e = malloc(sizeof(entity));
    *e = entity_factory_create_entity(&em->factory, prefab_name);

    e->id = em->last_id;
    ++em->last_id;

    hash_table_set(em->entities, copy_entity_id(e->id), e);

    return e;
}

entity *entity_manager_get_entity(const entity_manager *em, entity_id id)
{
    return hash_table_get(em->entities, &id);
}

void entity_manager_set_entity_tag(entity_manager *em, entity *e,
                                   const char *tag)
{
    hash_table_set(em->tagged_entities, copy_string(tag), copy_entity_id(e->id));
}

entity *entity_manager_get_entity_by_tag(const entity_manager *em, const char *tag)
{
    int* id = hash_table_get(em->tagged_entities, tag);
    if (id) {
        return entity_manager_get_entity(em, *id);
    }
    return NULL;
}
