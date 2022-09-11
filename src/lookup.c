#include "lookup.h"

static void err_msg(const char* msg)
{
    printf("%s\n", msg);
    exit(-1);
}

struct lookup_t* lookup_init(
    size_t capacity, 
    size_t data_size,
    int (*item_comparator)(const void*, const void*),
    void (*deallocator)(void*))
{
    struct lookup_t* lookup = malloc(sizeof(struct lookup_t));
    if (!lookup)
        err_msg("lookup_init: Couldn't allocate memory for new lookup table.");
    lookup->sorted = false;
    lookup->deallocator = deallocator;
    lookup->capacity = capacity;
    lookup->item_comparator = item_comparator;
    lookup->data_size = data_size;
    lookup->len = 0;
    lookup->data = malloc(data_size * capacity);
    if (!lookup->data)
        err_msg("lookup_init: Couldn't allocate memory for new lookup table.");

    return lookup;
}

void lookup_push(struct lookup_t* lookup, void* item)
{
    lookup->sorted = false;
    
    memcpy((char*)lookup->data + lookup->len * lookup->data_size, item, lookup->data_size);
    lookup->len++;
    if (lookup->len == lookup->capacity)
    {
        lookup->capacity *= 2;
        void* alloc = realloc(lookup->data, lookup->data_size * lookup->capacity);
        if (!alloc)
            err_msg("lookup_push: Couldn't allocate memory when expanding capacity.");
        lookup->data = alloc;
    }
}

void lookup_change_comparator(struct lookup_t* lookup, int (*item_comparator)(const void*, const void*))
{
    lookup->item_comparator = item_comparator;
    qsort(lookup->data, lookup->len, lookup->data_size, lookup->item_comparator);
    lookup->sorted = true;
}

void* lookup_find(struct lookup_t* lookup, void* item, size_t* n_results)
{
    *n_results = 0;

    if (!lookup->sorted)
    {
        qsort(lookup->data, lookup->len, lookup->data_size, lookup->item_comparator);
        lookup->sorted = true;
    }

    void* find = bsearch(item, lookup->data, lookup->len, lookup->data_size, lookup->item_comparator);
    if (!find)
        return NULL;

    *n_results += 1;
    size_t bsearch_idx = ((char*)find - (char*)lookup->data)/lookup->data_size;
    size_t lowest_idx = bsearch_idx;
    // check for duplicates
    for (size_t i = bsearch_idx + 1; i < lookup->len; ++i)
    {
        if (lookup->item_comparator(find, (char*)lookup->data + i * lookup->data_size) == 0)
            *n_results += 1;
        else
            break;
    }
    // bsearch is not guaranteed to find the first occurrence, so we
    // also check left side
    if (bsearch_idx > 0)
    {
        for (size_t i = bsearch_idx; i --> 0;)
        {
            if (lookup->item_comparator(find, (char*)lookup->data + i * lookup->data_size) == 0)
            {
                *n_results += 1;
                lowest_idx = i;
            }
            else
                break;
        }
    }

    void* ret = malloc(*n_results * lookup->data_size);
    if (!ret)
        err_msg("lookup_find: Couldn't allocate memory to return result.");
    
    for (size_t i = lowest_idx; i < lowest_idx + *n_results; ++i)
        memcpy((char*)ret + (i - lowest_idx) * lookup->data_size, (char*)lookup->data + i * lookup->data_size, lookup->data_size);

    return ret;
}

void lookup_free(struct lookup_t* lookup)
{

    // if custom deallocator, iterate each item and call deallocator
    if (lookup->deallocator)
        for (size_t i = 0; i < lookup->len; ++i)
            lookup->deallocator((char*)lookup->data + i * lookup->data_size);
    
    free(lookup->data);
    lookup->data = NULL;

    free(lookup);
    lookup = NULL;
}