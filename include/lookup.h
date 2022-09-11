#ifndef LOOKUP_H
#define LOOKUP_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

struct lookup_t
{
    void* data;
    int (*item_comparator)(const void*, const void*);
    void (*deallocator)(void*);
    size_t data_size;
    size_t len;
    size_t capacity;
    bool sorted;
};

struct lookup_t* lookup_init(
    size_t capacity, 
    size_t data_size,
    int (*item_comparator)(const void*, const void*),
    void (*deallocator)(void*));

void lookup_change_comparator(struct lookup_t* lookup, int (*item_comparator)(const void*, const void*));

void lookup_push(struct lookup_t* lookup, void* item);

void* lookup_find(struct lookup_t* lookup, void* item, size_t* n_results);

void lookup_free(struct lookup_t* lookup);

#endif