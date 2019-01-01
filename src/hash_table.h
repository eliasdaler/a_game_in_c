// generic hash_table
// implemented with great help from this article:
// http://www.craftinginterpreters.com/hash-tables.html
//
// supports void* keys and values... be careful with types!

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef uint32_t (*hasher_func_ptr)(const void *obj, size_t key_size);
typedef bool (*hash_table_compare_func_ptr)(const void *a, const void *b, size_t key_size);
typedef void (*hash_table_deleter_func_ptr)(void *obj);

typedef struct hash_table_entry {
    uint32_t hash;
    void *key;
    void *value;
} hash_table_entry;

typedef struct hash_table {
    hash_table_entry *entries;  // dynamically growing array
    int capacity;
    int entry_count;
    size_t key_size;  // only for hash maps with keys with no padding
    hasher_func_ptr hasher;
    hash_table_compare_func_ptr compare_func;
    hash_table_deleter_func_ptr key_deleter_func;
    hash_table_deleter_func_ptr value_deleter_func;
} hash_table;

hash_table *hash_table_new_custom(hasher_func_ptr hasher,
                                  hash_table_compare_func_ptr compare_func);

// create a hash map with keys which can be compared via memcmp (
hash_table *hash_table_new(size_t key_size);
hash_table *strkey_hash_table_new(void);

void hash_table_set(hash_table *ht, void *key, void *value);
void *hash_table_get(hash_table *ht, const void *key);

void hash_table_remove(hash_table *ht, const void *key);
void hash_table_remove_all(hash_table *ht);

void hash_table_free(hash_table *ht);
