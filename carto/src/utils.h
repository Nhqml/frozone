#pragma once

#ifdef __OpenBSD__
    #include <err.h>
#else
    #include <error.h>
#endif

#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
    {
        #ifdef __OpenBSD__
            errc(1, errno, "failed when trying to allocate %lu bytes of memory", size);
        #else
            error(1, errno, "failed when trying to allocate %lu bytes of memory", size);
        #endif
    }

    return ptr;
}

/**
** \brief Safe calloc wrapper
**
** \param nmemb Number of elements to allocate
** \param size Size of a single element
**
** \return void*
*/
static inline void* xcalloc(size_t nmemb, size_t size)
{
    void* ptr = calloc(nmemb, size);
    if (ptr == NULL)
    {
        #ifdef __OpenBSD__
            errc(1, errno, "failed when trying to allocate %lu bytes of memory", size);
        #else
            error(1, errno, "failed when trying to allocate %lu bytes of memory", size);
        #endif
    }

    return ptr;
}

/**
** \brief Safe reallocarray wrapper
**
** \param nmemb Number of elements to allocate
** \param size Size of a single element
**
** \return void*
*/
static inline void* xreallocarray(void* ptr, size_t nmemb, size_t size)
{
    void* new_ptr = reallocarray(ptr, nmemb, size);
    if (ptr == NULL)
    {
        #ifdef __OpenBSD__
            errc(1, errno, "failed when trying to allocate %lu bytes of memory", size);
        #else
            error(1, errno, "failed when trying to allocate %lu bytes of memory", size);
        #endif
    }

    return new_ptr;
}

/**
** \brief Safe strdup wrapper
**
** \param str String to copy
**
** \return void*
*/
static inline void* xstrdup(const char* str)
{
    void* dup_str = strdup(str);
    if (dup_str == NULL)
    {
        #ifdef __OpenBSD__
            errc(1, errno, "failed to duplicate string");
        #else
            error(1, errno, "failed to duplicate string");
        #endif
    }

    return dup_str;
}

/**
** \brief Call readlink and returns the resolved path (or NULL if a failure occurred)
**
** \param path The path
**
** \return char*
*/
static inline char* readlink_str(const char* path)
{
    char* s = xmalloc(PATH_MAX);

    int ret = readlink(path, s, PATH_MAX);

    if (ret == -1)
    {
        free(s);
        return NULL;
    }

    s[ret] = '\0';

    return s;
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
** \warning This does **NOT** free the elements stored in the array! But this will free the whole array as
*well as the struct
**
** \param a The array
*/
void array_free(Array* a);

/**
** \brief Destroy the array and what's inside
**
** \warning Make sure the pointers inside the array are allocated on the heap!
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
** \brief Returns an array of pointers to integers (int*) representing PIDs or file descriptors
**
** \param dir_path String representing a path to a directory (`/proc`, `/proc/<pid>/fd`)
** \return int**
*/
Array* get_num_dir_contents(char* dir_path);
