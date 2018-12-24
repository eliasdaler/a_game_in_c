#pragma once

#include "entity.h"
#include "entity_factory.h"

#include <stddef.h>

struct hash_table;
struct resource_manager;

typedef struct entity_manager {
    struct hash_table *entities;  // entity_id -> entity
    entity_id last_id;            // free id
    entity_factory factory;
    struct hash_table *tagged_entities;  // tag (char*) -> entity id
} entity_manager;

void entity_manager_init(entity_manager *em, struct resource_manager *rm);
void entity_manager_free(entity_manager *em);

entity *entity_manager_create_entity(entity_manager *em,
                                     const char *prefab_name);

entity *entity_manager_get_entity(const entity_manager *em, entity_id id);

void entity_manager_set_entity_tag(entity_manager *em, entity *e, const char *tag);

entity *entity_manager_get_entity_by_tag(const entity_manager *em,
                                         const char *tag);
