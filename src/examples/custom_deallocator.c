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

void string_dealloc(void* a)
{
    struct person_t _a = *(struct person_t*)a;
    free(_a.name);
    _a.name = NULL;
}

int main()
{
    // create lookup table based on names
    // pass our custom deallocator string_dealloc as we will dynamically allocate memory for the names
    struct lookup_t* lookup = lookup_init(10, sizeof(struct person_t), &name_comparator, &string_dealloc);
    
    struct person_t person;

    person.name = strdup("zach");
    person.age = 23;
    lookup_push(lookup, &person);

    person.name = strdup("john");
    person.age = 30;
    lookup_push(lookup, &person);

    person.name = strdup("alfred");
    person.age = 50;
    lookup_push(lookup, &person);

    person.name = strdup("adam");
    person.age = 23;
    lookup_push(lookup, &person);

    person.name = strdup("kyle");
    person.age = 10;
    lookup_push(lookup, &person);

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
    
    lookup_free(lookup); // this calls the custom deallocator and frees those strdup calls
    free(find);
    find = NULL;

    return 0;
}