#include "utils.h"

#include <stdlib.h>

Array* array_new(void)
{
    return array_with_capacity(ARRAY_DEFAULT_CAPACITY);
}

Array* array_with_capacity(size_t capacity)
{
    Array* a = malloc(sizeof(Array));
    a->array = malloc(capacity * sizeof(void*));
    a->size = 0;
    a->capacity = capacity;

    return a;
}

Array* array_push(Array* a, void* element)
{
    if (a->size == a->capacity)
    {
        a->capacity *= 2;
        a->array = realloc(a->array, a->capacity * sizeof(void*));
    }

    a->array[a->size] = element;
    ++a->size;

    return a;
}

void array_destroy(Array* a)
{
    free(a->array);
    free(a);
}

void** array_as_raw(Array* a)
{
    void** array = a->array;
    free(a);

    return array;
}
