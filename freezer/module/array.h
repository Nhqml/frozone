/* SPDX-License-Identifier: MIT */
/*
 * Copyright (C) 2022 Styvell Pidoux
 */

#ifndef ARRAY_H
#define ARRAY_H

#define ARRAY_DEFAULT_CAPACITY 10


/**
** \brief Dynamic array
*/
typedef struct
{
    void** array;
    unsigned int size;
    unsigned int capacity;
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
Array* array_with_capacity(unsigned int capacity);

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

struct array_uid
{
    unsigned int uid; // user id of the array
    Array *array; // array to store data
};


#endif
