#include "hash_table.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static char* copy_string(char* other)
{
    char* result = malloc(strlen(other) + 1);
    strcpy(result, other);
    return result;
}

static int* copy_int(int i)
{
    int *result = malloc(sizeof(int));
    *result = i;
    return result;
}

static void insert_test_value(hash_table* ht, char* key, int value)
{
    char *key_copy = copy_string(key);
    int *value_copy = copy_int(value);
    hash_table_set(ht, key_copy, value_copy);
}

static void check_value(hash_table* ht, char* key, int expected)
{
    int* val;
    val = hash_table_get(ht, key);
    assert(val && *val == expected);
}

void hash_table_test()
{
    int *val;

    hash_table *ht = strkey_hash_table_new();
    insert_test_value(ht, "apple", 5);

    insert_test_value(ht, "tomato", 1); // this should collide with "apple"
    check_value(ht, "apple", 5);
    check_value(ht, "tomato", 1); // check that inserted correctly

    insert_test_value(ht, "carrot", 2); // should readjust here

    // check that we didn't lose anything
    check_value(ht, "apple", 5);
    check_value(ht, "carrot", 2);
    check_value(ht, "tomato", 1);

    insert_test_value(ht, "potato", 42);
    insert_test_value(ht, "coconut", 5);

    // do values still insert well?
    check_value(ht, "potato", 42);
    check_value(ht, "coconut", 5);

    insert_test_value(ht, "apple", 8); // should replace old value
    check_value(ht, "apple", 8);

    // check for non-existing key
    val = hash_table_get(ht, "orange");
    assert(!val);

    // check hash table removal
    hash_table_remove(ht, "apple");
    val = hash_table_get(ht, "apple");
    assert(!val);

    // check that other stuff still is here
    check_value(ht, "carrot", 2);

    // delete table
    hash_table_free(ht);

    printf("All hash table tests passed!\n");
}
