// generic hash_table
// implemented with great help from this article:
// http://www.craftinginterpreters.com/hash-tables.html
//
// supports void* keys and values... be careful with types!

#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef uint32_t (*hasher_func_ptr)(void *obj);
typedef _Bool (*hash_table_compare_func_ptr)(void *a, void *b);

typedef struct hash_table_entry {
    uint32_t hash;
    void *key;
    void *value;
} hash_table_entry;

typedef struct hash_table {
    hash_table_entry *entries;  // dynamically growing array
    int capacity;
    int entry_count;
    hasher_func_ptr hasher;
    hash_table_compare_func_ptr compare_func;
} hash_table;

hash_table *hash_table_new(hasher_func_ptr hasher,
                           hash_table_compare_func_ptr compare_func);
hash_table *strkey_hash_table_new();

void hash_table_set(hash_table *ht, void *key, void *value);
void *hash_table_get(hash_table *ht, void *key);

void hash_table_remove(hash_table *ht, void *key);

void hash_table_free(hash_table *ht);
