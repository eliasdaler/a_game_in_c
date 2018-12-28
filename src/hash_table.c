#include "hash_table.h"

#include <stdlib.h>
#include <string.h>

#define HASH_TABLE_START_SIZE 4
#define GROWTH_FACTOR 2
#define LOAD_FACTOR 0.75

// The hash_table_entry is a "tombstone" if it has this hash + NULL key
#define TOMBSTONE_HASH 424242

// FNV-1a
static uint32_t fnv_1a_hash(const void *key, int length) {
    uint32_t hash = 2166136261u;

    const uint8_t *data = (const uint8_t*)key;
    for (int i = 0; i < length; i++) {
        hash ^= data[i];
        hash *= 16777619;
    }

    return hash;
}

// string comparator
static uint32_t string_hash(const void *obj, size_t key_size)
{
    return fnv_1a_hash(obj, strlen((const char*)obj));
}

// default comparator
static uint32_t default_hash(const void *obj, size_t key_size)
{
    return fnv_1a_hash(obj, key_size);
}

static bool default_compare(const void *obj1, const void *obj2, size_t key_size)
{
    return memcmp(obj1, obj2, key_size) == 0;
}

static bool string_compare(const void *str1, const void *str2, size_t key_size)
{
    return strcmp((const char*)str1, (const char*)str2) == 0;
}

static void hash_table_entry_make_empty(hash_table_entry *entry)
{
    entry->hash = 0;
    entry->key = NULL;
    entry->value = NULL;
}

static hash_table_entry* alloc_entries_table(int size)
{
    // don't want to use memset/calloc here, because NULL is not guaranteed to be zeroed out bits
    hash_table_entry *entries = malloc(size * sizeof(hash_table_entry));
    for (int i = 0; i < size; ++i) {
        hash_table_entry_make_empty(&entries[i]);
    }
    return entries;
}

static void hash_table_default_deleter(void *ptr)
{
    free(ptr);
}

hash_table* hash_table_new_custom(hasher_func_ptr hasher, hash_table_compare_func_ptr compare_func)
{
    hash_table *ht = malloc(sizeof(hash_table));

    ht->entry_count = 0;
    ht->key_size = 0;
    ht->capacity = HASH_TABLE_START_SIZE;
    ht->entries = alloc_entries_table(ht->capacity);

    ht->hasher = hasher;
    ht->compare_func = compare_func;

    ht->key_deleter_func = hash_table_default_deleter;
    ht->value_deleter_func = hash_table_default_deleter;
    return ht;
}

hash_table *hash_table_new(size_t key_size)
{
    hash_table *ht = hash_table_new_custom(default_hash, default_compare);
    ht->key_size = key_size;
    return ht;
}

hash_table* strkey_hash_table_new()
{
    return hash_table_new_custom(string_hash, string_compare);
}

static bool is_empty_entry(hash_table_entry* entry)
{
    return entry->hash == 0 && entry->key == NULL;
}

static bool is_tombstone_entry(hash_table_entry* entry)
{
    return entry->hash == TOMBSTONE_HASH && entry->key == NULL;
}

static void hash_table_entry_make_tombstone(hash_table_entry *entry)
{
    entry->hash = TOMBSTONE_HASH;
    entry->key = NULL;
    entry->value = NULL;
}

static void hash_table_entry_free(hash_table *ht, hash_table_entry *entry)
{
    ht->key_deleter_func(entry->key);
    ht->value_deleter_func(entry->value);

    --ht->entry_count;
    hash_table_entry_make_empty(entry);
}

static hash_table_entry* find_entry(hash_table *ht, const void *key)
{
    uint32_t hash = ht->hasher(key, ht->key_size);
    int index = hash % ht->capacity;
    for(;;) {
        hash_table_entry *entry = &ht->entries[index];
        if (is_empty_entry(entry) || is_tombstone_entry(entry)) {
            return entry;
        }

        if (!is_empty_entry(entry) &&
                entry->hash == hash &&
                ht->compare_func(key, entry->key, ht->key_size)) {
            return entry;
        }

        index = (index + 1) % ht->capacity; // loop through entries until we find a free slot
    }
}

static bool hash_table_needs_readjustment(hash_table *ht)
{
    return ht->entry_count + 1 > ht->capacity * LOAD_FACTOR;
}

static void hash_table_readjust(hash_table* ht)
{
    int old_capacity = ht->capacity;
    hash_table_entry *old_entries = ht->entries;

    ht->capacity *= GROWTH_FACTOR;
    ht->entries = alloc_entries_table(ht->capacity);
    ht->entry_count = 0;

    // re-add non-empty entries
    for (int i = 0; i < old_capacity; ++i) {
        hash_table_entry* entry = &old_entries[i];
        if (!is_empty_entry(entry)) {
            hash_table_set(ht, entry->key, entry->value);
        }
    }

    free(old_entries);
}

void hash_table_set(hash_table *ht, void *key, void *value)
{
    hash_table_entry *entry = find_entry(ht, key);
    if (!is_empty_entry(entry)) { // remove previous entry
        hash_table_entry_free(ht, entry);
    }

    uint32_t hash = ht->hasher(key, ht->key_size);
    entry->hash = hash;
    entry->key = key;
    entry->value = value;
    ++ht->entry_count;

    if (hash_table_needs_readjustment(ht)) {
        hash_table_readjust(ht);
    }
}

void* hash_table_get(hash_table *ht, const void *key)
{
    hash_table_entry *entry = find_entry(ht, key);
    if (!is_empty_entry(entry)) {
        return entry->value;
    }

    return NULL;
}

void hash_table_remove(hash_table *ht, const void *key)
{
    hash_table_entry *entry = find_entry(ht, key);
    if (!is_empty_entry(entry)) {
        hash_table_entry_free(ht, entry);
        hash_table_entry_make_tombstone(entry);
    }
}

void hash_table_remove_all(hash_table *ht)
{
    for (int i = 0; i < ht->capacity; ++i) {
        hash_table_entry *entry = &ht->entries[i];
        if (!is_empty_entry(entry) && !is_tombstone_entry(entry)) {
            hash_table_entry_free(ht, entry);
        }
    }
}


void hash_table_free(hash_table *ht)
{
    hash_table_remove_all(ht);

    free(ht->entries);
    free(ht);
}

