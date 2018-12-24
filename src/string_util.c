#include "string_util.h"

#include <stdlib.h>
#include <string.h>

char* copy_string(char* other)
{
    char* result = malloc(strlen(other) + 1);
    strcpy(result, other);
    return result;
}

void remove_ending_newline(char* str)
{
    str[strcspn(str, "\n")] = 0;
}

