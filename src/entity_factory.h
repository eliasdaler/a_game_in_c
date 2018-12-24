#pragma once

#include "entity.h"

struct hash_table;
struct resource_manager;

typedef struct entity_factory {
    struct hash_table *prefabs;  // prefab name -> prefab
} entity_factory;

void entity_factory_init(entity_factory *factory, struct resource_manager *rm);
void entity_factory_free(entity_factory *factory);

entity entity_factory_create_entity(entity_factory *factory,
                                    const char *prefab_name);
