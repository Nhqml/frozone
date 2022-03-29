#include "utils.h"

#include <dirent.h>
#include <stdlib.h>
#include <sys/types.h>

#include <stdio.h>

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
        a->array = xreallocarray(a->array, a->capacity, sizeof(void*));
    }

    a->array[a->size] = element;
    ++a->size;

    return a;
}

void array_free(Array* a)
{
    free(a->array);
    free(a);
}

void array_destroy(Array* a)
{
    for (size_t i = 0; i < a->size; ++i)
        free(a->array[i]);
    free(a->array);
    free(a);
}

void** array_as_raw(Array* a)
{
    void** array = a->array;
    free(a);

    return array;
}

Array* get_num_dir_contents(char* dir_path)
{
    DIR* dir = NULL;
    struct dirent* dir_cur = NULL;

    dir = opendir(dir_path);
    // Could not open dir
    if (dir == NULL)
        return NULL;

    Array* a = array_new();

    while ((dir_cur = readdir(dir)) != NULL)
    {
        puts("here");
        int id = atoi(dir_cur->d_name);

        // If pid is 0, atoi did not encounter a valid PID, ote that we do not need to read `/proc/self` since it's a
        // symlink to another directory
        if (id != 0)
        {
            int* id_ptr = xmalloc(sizeof(int));
            *id_ptr = id;
            array_push(a, id_ptr);
        }
    }

    closedir(dir);

    return a;
}
