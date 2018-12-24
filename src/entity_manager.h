#pragma once

#include "entity_factory.h"

#include <stddef.h>

struct hash_table;
struct resource_manager;

typedef size_t entity_id;

typedef struct entity_manager {
    struct hash_table *entities;
    entity_id last_id;  // free id
    entity_factory factory;
} entity_manager;

void entity_manager_init(entity_manager *em, struct resource_manager *rm);
void entity_manager_free(entity_manager *em);

struct entity *entity_manager_create_entity(entity_manager *em,
                                            const char *prefab_name);
struct entity *entity_manager_get_entity(const entity_manager *em,
                                         entity_id id);
