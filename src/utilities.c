#include "utilities.h"

#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#include "ALU/dispatcher.h"
#include "memory/memory.h"
#include "memory/table.h"
#include "state.h"


typedef unsigned char (*tobool_func_t)(avm_memcell *);
typedef char *(*tostring_func_t)(avm_memcell *);


// Error/Warning handling
void avm_warning(char *format, ...) {
    fprintf(stderr, "At line %u: ", code[pc].srcLine);
    va_list arguments;
    va_start(arguments, format);
    vfprintf(stderr, format, arguments);
    va_end(arguments);
    fprintf(stderr, "\n");
    return;
}

void avm_error(char *format, ...) {
    fprintf(stderr, "At line %u: ", code[pc].srcLine);
    va_list arguments;
    va_start(arguments, format);
    vfprintf(stderr, format, arguments);
    va_end(arguments);
    executionFinished = true;
    fprintf(stderr, "\n");
    return;
}


// Boolean utilities
tobool_func_t toboolFuncs[] = {number_tobool, string_tobool,   bool_tobool,
                               table_tobool,  userfunc_tobool, libfunc_tobool,
                               nil_tobool,    undef_tobool};

unsigned char number_tobool(avm_memcell *m) { return m->data.numVal != 0; }

unsigned char string_tobool(avm_memcell *m) { return m->data.strVal[0] != 0; }

unsigned char bool_tobool(avm_memcell *m) { return m->data.boolVal; }

unsigned char table_tobool(avm_memcell *m) { return 1; }

unsigned char userfunc_tobool(avm_memcell *m) { return 1; }

unsigned char libfunc_tobool(avm_memcell *m) { return 1; }

unsigned char nil_tobool(avm_memcell *m) { return 0; }

unsigned char undef_tobool(avm_memcell *m) {
    assert(0);
    return 0;
}

unsigned char avm_tobool(avm_memcell *m) {
    assert(m->type >= 0 && m->type < undef_m);
    return (*toboolFuncs[m->type])(m);
}


// Memcell to string utilities
char *typeStrings[] = {"number",   "string",  "bool", "table",
                       "userfunc", "libfunc", "nil",  "undef"};

tostring_func_t tostringFuncs[] = {
    number_tostring,   string_tostring,  bool_tostring, table_tostring,
    userfunc_tostring, libfunc_tostring, nil_tostring,  undef_tostring};

char *number_tostring(avm_memcell *m) {
    assert(m);
    char *result = malloc(20 * sizeof(char));
    sprintf(result, "%g", m->data.numVal);
    return result;
}

char *string_tostring(avm_memcell *m) {
    assert(m);
    char *result = malloc((strlen(m->data.strVal) + 1) * sizeof(char));
    sprintf(result, "%s", m->data.strVal);
    return result;
}

char *bool_tostring(avm_memcell *m) {
    assert(m);
    if (m->data.boolVal) return strdup("true");
    return strdup("false");
}

// TODO: check if better solution exists
char *table_tostring(avm_memcell *m) {
    assert(m);
    char *result = malloc(50 * sizeof(char));
    sprintf(result, "table %p with %u items", (void *)m->data.tableVal,
            m->data.tableVal->total);
    return result;
}

char *userfunc_tostring(avm_memcell *m) {
    assert(m);
    unsigned int tmp = m->data.funcVal;
    char *result = malloc((strlen(user_funcs[tmp].id) + 1) * sizeof(char));
    sprintf(result, "%s", user_funcs[tmp].id);

    return result;
}

char *libfunc_tostring(avm_memcell *m) {
    assert(m);
    char *result = malloc((strlen(m->data.libfuncVal) + 1) * sizeof(char));
    sprintf(result, "%s", m->data.libfuncVal);
    return result;
}

char *nil_tostring(avm_memcell *m) {
    assert(m);
    return strdup("nil");
}

char *undef_tostring(avm_memcell *m) {
    assert(m);
    return strdup("undefined");
}

char *avm_tostring(avm_memcell *m) {
    assert(m->type >= 0 && m->type <= undef_m);
    return (*tostringFuncs[m->type])(m);
}


// HashMap utilities
unsigned hash_string(void *string) {
    size_t ui;
    char *str = (char *)string;
    unsigned int uiHash = 0U;
    for (ui = 0U; str[ui] != '\0'; ui++) uiHash = uiHash * 65599 + str[ui];
    return uiHash;
}

bool compare_strings(void *str1, void *str2) {
    return strcmp((char *)str1, (char *)str2) != 0 ? false : true;
}

unsigned hash_num(void *num) { return (unsigned)*(double *)num; }

bool compare_nums(void *num1, void *num2) {
    return *(double *)num1 == *(double *)num2;
}

unsigned hash_bool(void *b) { return (unsigned)*(bool *)b; }

bool compare_bools(void *b1, void *b2) { return *(bool *)b1 == *(bool *)b2; }


unsigned hash_memcell(void *m) {
    avm_memcell *memcell = (avm_memcell *)m;
    switch (memcell->type) {
        case number_m:
            return hash_num(&memcell->data.numVal);
        case userfunc_m:
            return memcell->data.funcVal;
        case libfunc_m:
            return hash_string(memcell->data.libfuncVal);
        case table_m:
            return (unsigned)((uintptr_t)memcell->data.tableVal);
        case string_m:
            return hash_string(memcell->data.strVal);
        case bool_m:
            return hash_bool(&memcell->data.boolVal);
        default:
            assert(0);
    }
}

bool compare_memcell(void *m1, void *m2) {
    avm_memcell *memcell1 = (avm_memcell *)m1;
    avm_memcell *memcell2 = (avm_memcell *)m2;
    assert(memcell1->type == memcell2->type);
    switch (memcell1->type) {
        case number_m:
            return compare_nums(&memcell1->data.numVal, &memcell2->data.numVal);
        case userfunc_m:
            return memcell1->data.funcVal == memcell2->data.funcVal;
        case libfunc_m:
            return compare_strings(memcell1->data.libfuncVal,
                                   memcell2->data.libfuncVal);
        case table_m:
            return memcell1->data.tableVal == memcell2->data.tableVal;
        case string_m:
            return compare_strings(memcell1->data.strVal,
                                   memcell2->data.strVal);
        case bool_m:
            return compare_bools(&memcell1->data.boolVal,
                                 &memcell2->data.boolVal);
        default:
            assert(0);
    }
}


// Other utilities
UserFunc *avm_getfuncinfo(unsigned pc) {
    assert(code[pc].opcode == enterfunc_v);
    return &user_funcs[code[pc].result.val];
}
