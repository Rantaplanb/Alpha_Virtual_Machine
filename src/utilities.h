#ifndef AVM_UTILITIES_H
#define AVM_UTILITIES_H

#include "memory/memory.h"


// Error/Warning handling
void avm_warning(char *format, ...);
void avm_error(char *format, ...);


// Boolean utilities
unsigned char number_tobool(avm_memcell *m);
unsigned char string_tobool(avm_memcell *m);
unsigned char bool_tobool(avm_memcell *m);
unsigned char table_tobool(avm_memcell *m);
unsigned char userfunc_tobool(avm_memcell *m);
unsigned char libfunc_tobool(avm_memcell *m);
unsigned char nil_tobool(avm_memcell *m);
unsigned char undef_tobool(avm_memcell *m);
unsigned char avm_tobool(avm_memcell *m);


// Memcell to string utilities
extern char *typeStrings[];
char *number_tostring(avm_memcell *);
char *string_tostring(avm_memcell *);
char *bool_tostring(avm_memcell *);
char *table_tostring(avm_memcell *);
char *userfunc_tostring(avm_memcell *);
char *libfunc_tostring(avm_memcell *);
char *nil_tostring(avm_memcell *);
char *undef_tostring(avm_memcell *);
char *avm_tostring(avm_memcell *m);


// HashTable utilities
unsigned hash_string(void *string);
bool compare_strings(void *str1, void *str2);
unsigned hash_num(void *num);
bool compare_nums(void *num1, void *num2);
unsigned hash_bool(void *b);
bool compare_bools(void *b1, void *b2);
unsigned hash_memcell(void *m);
bool compare_memcell(void *m1, void *m2);


// Other utilities
UserFunc *avm_getfuncinfo(unsigned pc);

#endif
