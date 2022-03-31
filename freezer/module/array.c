/* SPDX-License-Identifier: MIT */
/*
 * Copyright (C) 2022 Styvell Pidoux
 */

#include "array.h"

#include <linux/slab.h>

Array* array_new(void)
{
    return array_with_capacity(ARRAY_DEFAULT_CAPACITY);
}

Array* array_with_capacity(unsigned int capacity)
{
    Array* a = kzalloc(sizeof(Array), GFP_KERNEL);

    if (a == NULL)
        return NULL;

    a->array = kzalloc(capacity * sizeof(void*), GFP_KERNEL);

    if (a->array == NULL)
        return NULL;

    a->size = 0;
    a->capacity = capacity;

    return a;
}

Array* array_push(Array* a, void* element)
{
    if (a->size == a->capacity)
    {
        a->capacity *= 2;
        a->array = krealloc(a->array, a->capacity * sizeof(void*), GFP_KERNEL);

        if (a->array == NULL)
            return NULL;

    }

    a->array[a->size] = element;
    ++a->size;

    return a;
}

void array_free(Array* a)
{
    kfree(a->array);
    kfree(a);
}

void array_destroy(Array* a)
{
    unsigned int i = 0;
    for (; i < a->size; ++i)
        kfree(a->array[i]);

    kfree(a->array);
    kfree(a);
}
