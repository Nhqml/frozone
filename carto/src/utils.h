#pragma once

#include <errno.h>
#include <error.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_DEFAULT_CAPACITY 10

/**
** \brief Safe malloc wrapper
**
** \param size Number of bytes to allocate
*/
static inline void* xmalloc(size_t size)
{
    void* ptr = malloc(size);
    if (ptr == NULL)
        error(1, errno, "failed when trying to allocate %lu bytes of memory", size);

    return ptr;
}

/**
** \brief Safe calloc wrapper
**
** \param nmemb Number of elements to allocate
** \param size Size of a single element
*/
static inline void* xcalloc(size_t nmemb, size_t size)
{
    void* ptr = calloc(nmemb, size);
    if (ptr == NULL)
        error(1, errno, "failed when trying to allocate %lu bytes of memory", size * nmemb);

    return ptr;
}

/**
** \brief Safe reallocarray wrapper
**
** \param nmemb Number of elements to allocate
** \param size Size of a single element
*/
static inline void* xreallocarray(void* ptr, size_t nmemb, size_t size)
{
    void* new_ptr = reallocarray(ptr, nmemb, size);
    if (ptr == NULL)
        error(1, errno, "failed when trying to allocate %lu bytes of memory", size * nmemb);

    return new_ptr;
}

/**
** \brief Safe strdup wrapper
**
** \param str String to copy
*/
static inline void* xstrdup(const char* str)
{
    void* dup_str = strdup(str);
    if (dup_str == NULL)
        error(1, errno, "failed to duplicate string");

    return dup_str;
}

/**
** \brief Dynamic array
*/
typedef struct
{
    void** array;
    size_t size;
    size_t capacity;
} Array;

/**
** \brief Create an array
**
** \return Array*
*/
Array* array_new(void);

/**
** \brief Create an array with a specific capacity
**
** \param capacity Initial capacity of the array
** \return Array*
*/
Array* array_with_capacity(size_t capacity);

/**
** \brief Add an element to the end of the array
**
** \param a The array
** \param element The element
** \return Array*
*/
Array* array_push(Array* a, void* element);

/**
** \brief Destroy the array
**
** **Warning** This does **NOT** free the elements stored in the array! But this will free the whole array as well as
** the struct
**
** \param a The array
*/
void array_destroy(Array* a);

/**
** \brief Returns the underlying "raw" C-array
**
** **Warning** It will only free the memory of the struct, not the memory of the actual array
** The caller is therefore responsible of freeing the array once it does not need it anymore
**
** \param a The array
** \return void**
*/
void** array_as_raw(Array* a);

/**
** \brief Returns an array of integers representing pids or file descriptors
**
** \param dir_path String representing a path to a directory
** \return int*
*/
int* get_num_dir_contents(char* dir_path);
