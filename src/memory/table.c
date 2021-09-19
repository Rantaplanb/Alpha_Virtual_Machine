#include "table.h"

#include <assert.h>
#include <stdlib.h>

#include "../libs/hash_map.h"
#include "../utilities.h"


memclear_func_t memclearFuncs[] = {
    0, /* number */
    memclear_string,
    0, /* bool */
    memclear_table,
    0, /* userfunc */
    0, /* libfunc */
    0, /* nil */
    0  /* undef */
};

avm_table *avm_tablenew(void) {
    avm_table *t = (avm_table *)malloc(sizeof(avm_table));
    AVM_WIPEOUT(*t);

    t->refCounter = 0;
    t->total = 0;
    t->strIndexed = HashMap_new(256, compare_memcell, hash_memcell);
    t->numIndexed = HashMap_new(256, compare_memcell, hash_memcell);
    t->boolIndexed = HashMap_new(2, compare_memcell, hash_memcell);
    t->tableIndexed = HashMap_new(256, compare_memcell, hash_memcell);
    t->userfuncIndexed = HashMap_new(256, compare_memcell, hash_memcell);
    t->libfuncIndexed = HashMap_new(256, compare_memcell, hash_memcell);

    return t;
}

static void avm_memcellclear_wrapper(void *m) { avm_memcellclear(m); }

void avm_tabledestroy(avm_table *t) {
    HashMap_apply_all(t->strIndexed, avm_memcellclear_wrapper,
                      avm_memcellclear_wrapper);
    HashMap_apply_all(t->numIndexed, avm_memcellclear_wrapper,
                      avm_memcellclear_wrapper);
    free(t);
}

void memclear_string(avm_memcell *m) {
    assert(m->data.strVal);
    free(m->data.strVal);
}


void memclear_table(avm_memcell *m) {
    assert(m->data.tableVal);
    avm_tabledecrefcounter(m->data.tableVal);
}

void avm_memcellclear(avm_memcell *m) {
    if (m->type != undef_m) {
        memclear_func_t f = memclearFuncs[m->type];
        if (f) (*f)(m);
        m->type = undef_m;
    }
}

// TODO: extensive testing
avm_memcell *avm_tablegetelem(avm_table *table, avm_memcell *index) {
    assert(index && table);
    if (index->type == nil_m || index->type == undef_m) {
        avm_error("unsupported indexing with type '%s'.",
                  typeStrings[index->type]);
        return NULL;
    }
    switch (index->type) {
        case number_m:
            return HashMap_get(table->numIndexed, index);
        case userfunc_m:
            return HashMap_get(table->userfuncIndexed, index);
        case libfunc_m:
            return HashMap_get(table->libfuncIndexed, index);
        case table_m:
            return HashMap_get(table->tableIndexed, index);
        case string_m:
            return HashMap_get(table->strIndexed, index);
        case bool_m:
            return HashMap_get(table->boolIndexed, index);
        default:
            assert(0);
    }
}

// TODO: extensive testing
void avm_tablesetelem(avm_table *table, avm_memcell *index,
                      avm_memcell *content) {
    assert(index && table);
    if (index->type == nil_m || index->type == undef_m) {
        avm_error("unsupported indexing with type '%s'.",
                  typeStrings[index->type]);
        return;
    }
    avm_memcell *datum = calloc(sizeof(avm_memcell), 1);
    avm_memcell *key = calloc(sizeof(avm_memcell), 1);
    *datum = *content;
    if (datum->type == table_m) avm_tableincrefcounter(datum->data.tableVal);
    if (content->type != nil_m) {
        switch (index->type) {
            case number_m:
                key->type = number_m;
                key->data.numVal = index->data.numVal;
                HashMap_insert(table->numIndexed, key, datum);
                break;
            case userfunc_m:
                key->type = userfunc_m;
                key->data.funcVal = index->data.funcVal;
                HashMap_insert(table->userfuncIndexed, key, datum);
                break;
            case libfunc_m:
                key->type = libfunc_m;
                key->data.libfuncVal = strdup(index->data.libfuncVal);
                HashMap_insert(table->libfuncIndexed, key, datum);
                break;
            case table_m:
                key->type = table_m;
                key->data.tableVal = index->data.tableVal;
                HashMap_insert(table->tableIndexed, key, datum);
                break;
            case string_m:
                key->type = string_m;
                key->data.strVal = strdup(index->data.strVal);
                HashMap_insert(table->strIndexed, key, datum);
                break;
            case bool_m:
                key->type = bool_m;
                key->data.boolVal = index->data.boolVal;
                HashMap_insert(table->boolIndexed, key, datum);
                break;
            default:
                assert(0);
        }
    } else {
        switch (index->type) {
            case number_m:
                free(key);
                HashMap_remove(table->numIndexed, index);
                break;
            case userfunc_m:
                free(key);
                HashMap_remove(table->userfuncIndexed, index);
                break;
            case libfunc_m:
                free(key);
                HashMap_remove(table->libfuncIndexed, index);
                break;
            case table_m:
                free(key);
                HashMap_remove(table->tableIndexed, index);
                break;
            case string_m:
                free(key);
                HashMap_remove(table->strIndexed, index);
                break;
            case bool_m:
                free(key);
                HashMap_remove(table->boolIndexed, index);
                break;
            default:
                assert(0);
        }
    }

    table->total =
        HashMap_count(table->numIndexed) + HashMap_count(table->strIndexed);
}

void avm_tableincrefcounter(avm_table *t) { ++t->refCounter; }

void avm_tabledecrefcounter(avm_table *t) {
    assert(t->refCounter > 0);
    if (!--t->refCounter) avm_tabledestroy(t);
}
