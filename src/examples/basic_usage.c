#include "lookup.h"

struct person_t
{
    char* name;
    int age;
};

int name_comparator(const void* a, const void* b)
{
    const struct person_t _a = *(const struct person_t*)a;
    const struct person_t _b = *(const struct person_t*)b;

    return strcmp(_a.name, _b.name);
}

int age_comparator(const void* a, const void* b)
{
    const struct person_t _a = *(const struct person_t*)a;
    const struct person_t _b = *(const struct person_t*)b;

    return _a.age - _b.age;
}

int main()
{
    // create lookup table based on names
    struct lookup_t* lookup = lookup_init(10, sizeof(struct person_t), &name_comparator, NULL);
    lookup_push(lookup, &(struct person_t){.name = "zach", .age = 23});
    lookup_push(lookup, &(struct person_t){.name = "john", .age = 40});
    lookup_push(lookup, &(struct person_t){.name = "bill", .age = 12});
    lookup_push(lookup, &(struct person_t){.name = "kyle", .age = 33});
    lookup_push(lookup, &(struct person_t){.name = "jeffrey", .age = 84});
    lookup_push(lookup, &(struct person_t){.name = "adam", .age = 32});
    lookup_push(lookup, &(struct person_t){.name = "john", .age = 25});
    lookup_push(lookup, &(struct person_t){.name = "chris", .age = 9});
    lookup_push(lookup, &(struct person_t){.name = "aleph", .age = 29});
    lookup_push(lookup, &(struct person_t){.name = "john", .age = 23});

    // lookup table supports duplicate keys, so we can iterate them using n_results
    size_t n_results = 0;
    struct person_t search_key = {.name = "john"};
    struct person_t* find = lookup_find(lookup, &search_key, &n_results);
    if (find)
    {
        for (size_t i = 0; i < n_results; ++i)
        {
            printf("Found Name: %s\n", find[i].name);
            printf("Found Age: %d\n", find[i].age);
        }
    }
    else
        printf("Person '%s' not found!\n", search_key.name);

    printf("\n\n");

    // change comparator to age if we want to search on something else
    // NOTE: this will cause a re-sort
    lookup_change_comparator(lookup, &age_comparator);
    search_key.age = 23;
    free(find); // free previous search before searching again!!
    find = lookup_find(lookup, &search_key, &n_results);
    if (find)
    {
        for (size_t i = 0; i < n_results; ++i)
        {
            printf("Found Name: %s\n", find[i].name);
            printf("Found Age: %d\n", find[i].age);
        }
    }
    else
        printf("Person '%s' not found!\n", search_key.name);
    
    lookup_free(lookup);
    free(find);
    find = NULL;

    return 0;
}