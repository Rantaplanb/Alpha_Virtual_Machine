#include "hash_map.h"

#include <stdlib.h>


static DatumWrapper *DatumWrapper_new(void *key, void *datum);


/*
 * Make and return a new hash map with given compare and hash functions.
 */
HashMap *HashMap_new(unsigned buckets, bool (*compare)(void *, void *),
                     unsigned (*hash)(void *)) {
    HashMap *hash_map = (HashMap *)malloc(sizeof(struct HashMap));
    hash_map->buckets = buckets;
    hash_map->compare = compare;
    hash_map->hash = hash;
    hash_map->count = 0;
    hash_map->map = calloc(sizeof(DatumWrapper), buckets);

    return hash_map;
}


/*
 * Insert a data node with given key in the hash map.
 * If given key has already been used return false, else insert the new node
 * and return true.
 */
bool HashMap_insert(HashMap *hash_map, void *key, void *datum) {
    DatumWrapper *iter, *prev = NULL;
    unsigned int cell_pos;

    cell_pos = hash_map->hash(key) % hash_map->buckets;
    iter = hash_map->map[cell_pos];

    while (iter) {
        if (hash_map->compare(key, iter->key)) {
            iter->datum = datum;
            return false;
        }
        prev = iter;
        iter = iter->next;
    }

    if (prev)
        prev->next = DatumWrapper_new(key, datum);
    else
        hash_map->map[cell_pos] = DatumWrapper_new(key, datum);

    (hash_map->count)++;

    return 1;
}

/*
 * Remove a data node with given key in the hash map.
 * If given key has already been used return false, else insert the new node
 * and return true.
 */
bool HashMap_remove(HashMap *hash_map, void *key) {
    DatumWrapper *iter, *prev = NULL;
    unsigned int cell_pos;

    cell_pos = hash_map->hash(key) % hash_map->buckets;
    iter = hash_map->map[cell_pos];

    while (iter) {
        if (hash_map->compare(key, iter->key)) break;

        prev = iter;
        iter = iter->next;
    }

    if (iter) {
        (hash_map->count)--;
        if (prev) {
            free(iter->key);
            prev->next = iter->next;
        } else {
            free(iter->key);
            hash_map->map[cell_pos] = NULL;
        }
        return true;
    }


    return false;
}

/*
 * Get entry with given key from the hash map.
 * Return NULL if the entry doesn't exist.
 */
void *HashMap_get(HashMap *hash_map, void *key) {
    DatumWrapper *iter;
    unsigned int cell_pos;

    cell_pos = hash_map->hash(key) % hash_map->buckets;
    iter = hash_map->map[cell_pos];

    while (iter && !hash_map->compare(key, iter->key)) iter = iter->next;

    if (iter)
        return iter->datum;
    else
        return NULL;
}


/*
 * Get the count of unique entries in the given hash_map.
 */
unsigned HashMap_count(HashMap *hash_map) { return hash_map->count; }


/*
 * Delete given HashMap.
 * Does not free the entries themselves, this should be done externally.
 */
void HashMap_destruct(HashMap *hash_map) {
    DatumWrapper *iter, *next;
    unsigned int i;

    for (i = 0; i < hash_map->buckets; i++) {
        iter = hash_map->map[i];

        while (iter) {
            next = iter->next;
            iter->next = NULL;
            free(iter);

            iter = next;
        }
    }

    free(hash_map->hash);
    free(hash_map);

    return;
}


/*
 * Initialize a datum wrapper.
 */
static DatumWrapper *DatumWrapper_new(void *key, void *datum) {
    DatumWrapper *datum_wrapper = (DatumWrapper *)malloc(sizeof(DatumWrapper));

    datum_wrapper->key = key;
    datum_wrapper->datum = datum;
    datum_wrapper->next = NULL;

    return datum_wrapper;
}


/*
 * Apply given functions on all members of the given HashMap.
 */
void HashMap_apply_all(HashMap *hash_map, void (*index_func)(void *),
                       void (*datum_func)(void *)) {
    DatumWrapper *wrapper;
    for (size_t i = 0; i < hash_map->buckets; ++i) {
        wrapper = hash_map->map[i];
        while (wrapper) {
            index_func(wrapper->key);
            datum_func(wrapper->datum);
            wrapper = wrapper->next;
        }
    }
}
