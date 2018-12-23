#include "hash_table.h"

#include <stdlib.h>
#include <string.h>

#define HASH_TABLE_START_SIZE 4
#define GROWTH_FACTOR 2
#define LOAD_FACTOR 0.75

// The hash_table_entry is a "tombstone" if it has this hash + NULL key
#define TOMBSTONE_HASH 424242

// FNV-1a
static uint32_t fnv_1a_hash(void *key, int length) {
    uint32_t hash = 2166136261u;

    const uint8_t *data = (const uint8_t*)key;
    for (int i = 0; i < length; i++) {
        hash ^= data[i];
        hash *= 16777619;
    }

    return hash;
}

// string comparator
static uint32_t string_hash(void *obj)
{
    return fnv_1a_hash(obj, strlen((char*)obj));
}

static _Bool string_compare(void *str1, void *str2)
{
    return strcmp((char*)str1, (char*)str2) == 0;
}

static hash_table_entry* alloc_entries_table(int size)
{
    // don't want to use memset/calloc here, because NULL is not guaranteed to be zeroed out bits
    hash_table_entry *entries = malloc(size * sizeof(hash_table_entry));
    for (int i = 0; i < size; ++i) {
        entries[i] = (hash_table_entry) { .hash = 0, .key = NULL, .value = NULL };
    }
    return entries;
}

hash_table* hash_table_new(hasher_func_ptr hasher, hash_table_compare_func_ptr compare_func)
{
    hash_table *ht = malloc(sizeof(hash_table));

    ht->entry_count = 0;
    ht->capacity = HASH_TABLE_START_SIZE;
    ht->entries = alloc_entries_table(ht->capacity);

    ht->hasher = hasher;
    ht->compare_func = compare_func;
    return ht;
}

hash_table* strkey_hash_table_new()
{
    return hash_table_new(string_hash, string_compare);
}

static void hash_table_entry_free(hash_table_entry *entry)
{
    entry->hash = 0;

    free(entry->key);
    entry->key = NULL;

    free(entry->value);
    entry->value = NULL;
}

static _Bool is_empty_entry(hash_table_entry* entry)
{
    return entry->hash == 0 && entry->key == NULL;
}

static _Bool is_tombstone_entry(hash_table_entry* entry)
{
    return entry->hash == TOMBSTONE_HASH && entry->key == NULL;
}

static void hash_table_entry_make_tombstone(hash_table_entry *entry)
{
    hash_table_entry_free(entry);
    entry->hash = TOMBSTONE_HASH;
}

static hash_table_entry* find_entry(hash_table *ht, void *key)
{
    uint32_t hash = ht->hasher(key);
    int index = hash % ht->capacity;
    for(;;) {
        hash_table_entry *entry = &ht->entries[index];
        if (is_empty_entry(entry) || is_tombstone_entry(entry)) {
            return entry;
        }

        if (!is_empty_entry(entry) &&
                entry->hash == hash &&
                ht->compare_func(key, entry->key)) {
            return entry;
        }

        index = (index + 1) % ht->capacity; // loop through entries until we find a free slot
    }
}

static _Bool hash_table_needs_readjustment(hash_table *ht)
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
        hash_table_entry_free(entry);
    } else {
        ++ht->entry_count;
    }

    uint32_t hash = ht->hasher(key);
    entry->hash = hash;
    entry->key = key;
    entry->value = value;

    if (hash_table_needs_readjustment(ht)) {
        hash_table_readjust(ht);
    }
}

void* hash_table_get(hash_table *ht, void *key)
{
    hash_table_entry *entry = find_entry(ht, key);
    if (!is_empty_entry(entry)) {
        return entry->value;
    }

    return NULL;
}

void hash_table_remove(hash_table *ht, void *key)
{
    hash_table_entry *entry = find_entry(ht, key);
    if (!is_empty_entry(entry)) {
        hash_table_entry_make_tombstone(entry);
    }
}

void hash_table_free(hash_table *ht)
{
    for (int i = 0; i < ht->capacity; ++i) {
        hash_table_entry *entry = &ht->entries[i];
        if (!is_empty_entry(entry)) {
            hash_table_entry_free(entry);
        }
    }

    free(ht->entries);
    free(ht);
}

