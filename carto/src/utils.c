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

int* get_num_dir_contents(char* dir_path)
{
    DIR* dir = NULL;
    struct dirent* dir_cur = NULL;

    dir = opendir(dir_path);
    
    // log instead of error
    if (dir == NULL)
        error(1, errno, "failed to open %s directory", dir_path);
    
    Array* a = array_new();

    while ((dir_cur = readdir(dir)) != NULL)
    {
        int id = atoi(dir_cur->d_name);

        // if pid is 0, atoi did not encounter a valid number
        if (id)
            array_push(a, id);
    }

    closedir(dir);

    array_push(a, NULL);

    return (int*)array_as_raw(a);
}
