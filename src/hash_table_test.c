#include "hash_table.h"

#include "string_util.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static int custom_deleter_called_num_times = 0;

void custom_deleter(void *ptr)
{
    ++custom_deleter_called_num_times;
    free(ptr);
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

void hash_table_test(void)
{
    int *val;

    hash_table *ht = strkey_hash_table_new();
    assert(ht->entry_count == 0);

    insert_test_value(ht, "apple", 5);
    insert_test_value(ht, "tomato", 1); // this should collide with "apple"
    assert(ht->entry_count == 2);

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

    assert(ht->entry_count == 5);
    insert_test_value(ht, "apple", 8); // should replace old value
    assert(ht->entry_count == 5);
    check_value(ht, "apple", 8);

    // check for non-existing key
    val = hash_table_get(ht, "orange");
    assert(!val);

    // check hash table removal
    hash_table_remove(ht, "apple");
    assert(ht->entry_count == 4);
    val = hash_table_get(ht, "apple");
    assert(!val);

    // check that other stuff still is here
    check_value(ht, "carrot", 2);

    // set custom deleters for test
    ht->key_deleter_func = custom_deleter;
    ht->value_deleter_func = custom_deleter;
    int old_entry_count = ht->entry_count;

    hash_table_remove_all(ht);

    assert(ht->entry_count == 0);
    assert(old_entry_count * 2 == 8); // 4 values remain - called for both keys and values

    hash_table_free(ht);


    // test hash_table with ints
    hash_table* ht2 = hash_table_new(sizeof(int));

    hash_table_set(ht2, copy_int(0), copy_int(42));
    hash_table_set(ht2, copy_int(42), copy_int(-52));
    hash_table_set(ht2, copy_int(-32), copy_int(0));
    hash_table_set(ht2, copy_int(190), copy_int(1337));

    assert(ht2->entry_count == 4);

    // check for existing key
    int test_number = 42;
    val = hash_table_get(ht2, &test_number);
    assert(val && *val == -52);

    // check for non-existing key
    test_number = 1;
    val = hash_table_get(ht2, &test_number);
    assert(!val);

    hash_table_free(ht);

    printf("All hash table tests passed!\n");
}
