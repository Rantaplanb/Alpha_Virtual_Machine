#ifndef AVM_HASH_MAP_H
#define AVM_HASH_MAP_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct DatumWrapper {
    void *key;
    void *datum;
    struct DatumWrapper *next;
} DatumWrapper;

typedef struct HashMap {
    bool (*compare)(void *, void *);
    unsigned (*hash)(void *);
    unsigned buckets;
    unsigned count;
    DatumWrapper **map;
} HashMap;


/*
 * Make and return a new hash map with given compare and hash functions.
 */
HashMap *HashMap_new(unsigned buckets, bool (*compare)(void *, void *),
                     unsigned (*hash)(void *));

/*
 * Insert a data node with given key in the hash map.
 * If given key has already been used return false, else insert the new node
 * and return true.
 */
bool HashMap_insert(HashMap *HashMap, void *key, void *datum);


/*
 * Remove a data node with given key in the hash map.
 * If given key has already been used return false, else insert the new node
 * and return true.
 */
bool HashMap_remove(HashMap *hash_map, void *key);

/*
 * Get entry with given key from the hash map.
 * Return NULL if the entry doesn't exist.
 */
void *HashMap_get(HashMap *HashMap, void *key);

/*
 * Get the count of unique entries in the given hash_map.
 */
unsigned HashMap_count(HashMap *hash_map);

/*
 * Delete given HashMap.
 * Does not free the entries themselves, this should be done externally.
 */
void HashMap_destruct(HashMap *HashMap);

/*
 * Apply given functions on all members of the given HashMap.
 */
void HashMap_apply_all(HashMap *hash_map, void (*index_func)(void *),
                       void (*datum_func)(void *));

#endif  // AVM_HASH_MAP_H
