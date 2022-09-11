# Lookup Table Data Structure
## Quick Description
This is a generic, dynamically-sized lookup table data structure designed for inserting chunks of data at once and performing multiple queries against it. It's essentially a fancy wrapper around the standard library's `qsort()` and `bsearch()` functions with some extra conveniences.

This data structure is sorted on demand, meaning the lookup table is NOT sorted until one of these things happen:
* user calls `lookup_find`
* user calls `lookup_change_comparator`

A binary search is performed when calling `lookup_find`. If duplicate keys are present, all records with that key are returned.

## Time Complexity
* Inserting (`lookup_push`)
  * O(1)
* Fetching (`lookup_find`)
  * O(n log n) on first call
  * O(log n) on subsequent calls

## Custom Deallocators
This data structure also supports providing custom deallocators incase you have structures containing dynamically-allocated memory. See [this example](src/examples/custom_deallocator.c) for a preview of this.

## Usage Notes
This data structure is designed for inserting large chunks of data at once to later run many queries against it. In these scenarios, it will perform its best.

This data structure is NOT designed for rapid push and search. I.e., many push and find calls back to back. This is because the data is sorted on every call to `lookup_find`.

It's recommended to design your code to insert as much data as possible up front before querying records.

## API

### Initializer
This will create a new lookup table. User provides an initial capacity but this grows dynamically as your table gets larger and larger. If you are expecting lots of data, you can avoid unnecessary reallocations by setting a high capacity.
```c
struct lookup_t* lookup_init(
    // initial capacity (this dynamically grows)
    size_t capacity,
    // size of your struct, e.g., sizeof(struct mystruct)
    size_t data_size,
    // comparison functin used in sorting & searching.
    //This is passed to qsort() and bsearch()
    int (*item_comparator)(const void*, const void*),
    // custom deallocator if your struct contains
    // dynamically-allocated memory
    void (*deallocator)(void*));
```

### Push
This pushes a (stack-allocated) struck into the lookup table where the contents are copied. This way, you can use the same stack-allocated struct to push multiple records into the lookup table without having to waste space.
```c
void lookup_push(
    // your lookup table
    struct lookup_t* lookup,
    // address of a local struct to insert into table
    void* item);
```

## Find
Provide a (stack-allocated) struct containing the key values to search for according to the provided comparator function. If an object is found, it returns a pointer to the first occurrence of the key as well as recording the number of instances found in `n_results` in the case of duplicates. This can then be treated as an array of structs by the user. If an object is not found, NULL is returned.
```c
void* lookup_find(
    // your lookup table
    struct lookup_t* lookup,
    // address of local struct containing the key values to search
    void* item, 
    // address of local size_t variable. This is modified by the function recording how many instances were found for the given key (in the case of duplicates)
    size_t* n_results);
```

## Change Comparator
Change the comparison function used for sorting & searching keys. Used if you want to change how to organize the internal structure of the lookup table for searching different keys. Calling this function will force a re-sort.
```c
void lookup_change_comparator(
    // your lookup table
    struct lookup_t* lookup, 
    // new comparison function to use (passed to qsort() and bsearch())
    int (*item_comparator)(const void*, const void*));
```

## Free
Cleanup memory allocated by the lookup table. If you provided a custom deallocator, this will be called on each element stored in the lookup table.
```c
void lookup_free(
    // your lookup table
    struct lookup_t* lookup);
```

## Examples
For a list of examples, see [here](src/examples)

## Building from Source
Git clone this repository and navigate to the root directory and make your build folder, e.g, `mkdir build && cd build`.

There are two CMake options used for this project:
* `CMAKE_BUILD_TYPE`
  * `Debug` - Uses flags `-O0 -g -Wall`
  * `Release` - Uses flags `-O2 -Wall` **(default)**
* `EXAMPLES`
  * `OFF` - don't compile examples **(default)**
  * `ON` - compile examples (stored in `src/examples`)

A full example command would be:

`cmake -DCMAKE_BUILD_TYPE=Debug -DEXAMPLES=ON ..`

If you don't provide `-DCMAKE_BUILD_TYPE` flag, it will be `Release` by default.

If you don't provide `-DEXAMPLES` flag, it will be `OFF` by default.


## Including in CMake Projects
1. (OPTIONAL) Create a folder to store dependencies (e.g., `ext/`)
2. Add this repo as a submodule
   1. `git submodule add [this repo link]`
3. In your root CMakeLists, add the subdirectory and include the headers and link the library to your target
   1. `add_subdirectory(ext/lookup)`
   2. `target_include_directories([your target] PUBLIC ${lookup_SOURCE_DIR}/include)`
   3. `target_link_libraries([your target] lookup)`

Now this should be a part of your build.
