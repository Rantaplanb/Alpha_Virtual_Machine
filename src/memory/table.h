#ifndef AVM_TABLE_HASHSIZE
#define AVM_TABLE_HASHSIZE 211

#include "memory.h"

typedef struct avm_table_bucket avm_table_bucket;

typedef void (*memclear_func_t)(avm_memcell *);

struct avm_table_bucket {
    avm_memcell key;
    avm_memcell value;
    avm_table_bucket *next;
};

struct avm_table {
    unsigned refCounter;
    HashMap *strIndexed;
    HashMap *numIndexed;
    HashMap *boolIndexed;
    HashMap *tableIndexed;
    HashMap *userfuncIndexed;
    HashMap *libfuncIndexed;
    unsigned int total;
};

avm_table *avm_tablenew(void);
void avm_memcellclear(avm_memcell *m);
avm_memcell *avm_tablegetelem(avm_table *table, avm_memcell *index);
void avm_tablesetelem(avm_table *table, avm_memcell *index,
                      avm_memcell *content);
void avm_tabledestroy(avm_table *t);
void avm_tableincrefcounter(avm_table *t);
void avm_tabledecrefcounter(avm_table *t);
void avm_tablebucketsinit(avm_table_bucket **p);
void avm_tablebucketsdestroy(avm_table_bucket **p);
void memclear_string(avm_memcell *m);
void memclear_table(avm_memcell *m);


#endif
