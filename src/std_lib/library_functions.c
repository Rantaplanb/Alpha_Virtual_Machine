#include "library_functions.h"

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../ALU/exec_branches.h"
#include "../ALU/exec_functions.h"
#include "../libs/hash_map.h"
#include "../memory/memory.h"
#include "../memory/stack.h"
#include "../memory/table.h"
#include "../utilities.h"

#define PI 3.141592


void libfunc_cos(void) {
    unsigned int actuals = avm_totalactuals();
    if (actuals != 1) {
        avm_error(
            "Invalid use of library function cosine. Can only be "
            "called with 1 argument.\n");
        return;
    }
    avm_memcell *argument = avm_getactual(0);
    if (argument->type != number_m) {
        avm_error("Cosine expected a number argument.\n");
    } else {
        double rad = argument->data.numVal * PI / 180;
        retval.type = number_m;
        retval.data.numVal = cos(rad);
    }
    return;
}

void libfunc_sin(void) {
    unsigned int actuals = avm_totalactuals();
    if (actuals != 1) {
        avm_error(
            "Invalid use of library function sine. Can only be "
            "called with 1 argument.\n");
        return;
    }
    avm_memcell *argument = avm_getactual(0);
    if (argument->type != number_m) {
        avm_error("Sine expected a number argument.\n");
    } else {
        double rad = argument->data.numVal * PI / 180;
        retval.type = number_m;
        retval.data.numVal = sin(rad);
    }
    return;
}

void libfunc_sqrt() {
    unsigned int actuals = avm_totalactuals();
    if (actuals != 1) {
        avm_error(
            "Invalid use of library function sqrt. Can only be "
            "called with 1 argument.\n");
        return;
    }
    avm_memcell *argument = avm_getactual(0);
    if (argument->type != number_m) {
        avm_error("Sqrt expected a number argument.\n");
    } else {
        if (argument->data.numVal < 0) {
            avm_memcellclear(&retval);
            retval.type = nil_m;
            return;
        }
        retval.type = number_m;
        retval.data.numVal = sqrt(argument->data.numVal);
    }
    return;
}

void libfunc_strtonum() {
    unsigned int actuals = avm_totalactuals();
    if (actuals != 1) {
        avm_error(
            "Invalid use of library function strtonum. Can only be "
            "called with 1 argument.\n");
        avm_memcellclear(&retval);
        retval.type = nil_m;
        return;
    }
    avm_memcell *argument = avm_getactual(0);
    if (argument->type != string_m) {
        avm_error("strtonum expected a string argument.\n");
    } else {
        retval.type = number_m;
        retval.data.numVal = atof(argument->data.strVal);
        if (retval.data.numVal == 0) {
            avm_memcellclear(&retval);
            retval.type = nil_m;
            return;
        }
    }
    return;
}

void memcpy_wrapper(void *item) {
    avm_memcell *tmp = item, *new;
    switch (tmp->type) {
        case number_m:
            new = calloc(sizeof(avm_memcell), 1);
            new->type = number_m;
            new->data.numVal = tmp->data.numVal;
            break;
        case string_m:
            new = calloc(sizeof(avm_memcell), 1);
            new->type = string_m;
            new->data.strVal = strdup(tmp->data.strVal);
            break;
        case bool_m:
            new = calloc(sizeof(avm_memcell), 1);
            new->type = bool_m;
            new->data.boolVal = tmp->data.boolVal;
            break;
        case table_m:
            new = calloc(sizeof(avm_memcell), 1);
            new->type = table_m;
            new->data.tableVal = tmp->data.tableVal;
            avm_tableincrefcounter(new->data.tableVal);
            break;
        case userfunc_m:
            new = calloc(sizeof(avm_memcell), 1);
            new->type = userfunc_m;
            new->data.funcVal = tmp->data.funcVal;
            break;
        case libfunc_m:
            new = calloc(sizeof(avm_memcell), 1);
            new->type = libfunc_m;
            new->data.libfuncVal = strdup(tmp->data.libfuncVal);
            break;
        default:
            return;
    }
    item = new;
}

void noop(void *item) { return; }

void libfunc_objectcopy() {
    unsigned int actuals = avm_totalactuals();
    if (actuals != 1) {
        avm_error(
            "Invalid use of library function object copy. Can only be "
            "called with 1 argument.\n");
        avm_memcellclear(&retval);
        retval.type = nil_m;
        return;
    }
    avm_memcell *argument = avm_getactual(0);
    if (argument->type != table_m) {
        avm_error("objectcopy expected a table argument.\n");
        return;
    }
    avm_table *table = argument->data.tableVal;
    retval.type = table_m;
    retval.data.tableVal = avm_tablenew();

    memcpy(retval.data.tableVal->numIndexed, table->numIndexed,
           sizeof(HashMap));
    memcpy(retval.data.tableVal->strIndexed, table->strIndexed,
           sizeof(HashMap));
    memcpy(retval.data.tableVal->boolIndexed, table->boolIndexed,
           sizeof(HashMap));
    memcpy(retval.data.tableVal->tableIndexed, table->tableIndexed,
           sizeof(HashMap));
    memcpy(retval.data.tableVal->userfuncIndexed, table->userfuncIndexed,
           sizeof(HashMap));
    memcpy(retval.data.tableVal->libfuncIndexed, table->libfuncIndexed,
           sizeof(HashMap));
    HashMap_apply_all(retval.data.tableVal->numIndexed, memcpy_wrapper,
                      memcpy_wrapper);
    HashMap_apply_all(retval.data.tableVal->strIndexed, memcpy_wrapper,
                      memcpy_wrapper);
    HashMap_apply_all(retval.data.tableVal->boolIndexed, memcpy_wrapper,
                      memcpy_wrapper);
    HashMap_apply_all(retval.data.tableVal->tableIndexed, memcpy_wrapper,
                      memcpy_wrapper);
    HashMap_apply_all(retval.data.tableVal->userfuncIndexed, memcpy_wrapper,
                      memcpy_wrapper);
    HashMap_apply_all(retval.data.tableVal->libfuncIndexed, memcpy_wrapper,
                      memcpy_wrapper);
}

static unsigned int counter = 0;
void wrapper_insert(void *item) {
    avm_memcell *index = calloc(sizeof(avm_memcell), 1);
    index->type = number_m;
    index->data.numVal = counter;
    HashMap_insert(retval.data.tableVal->numIndexed, index, item);
    counter++;
}

void libfunc_objectmemberkeys() {
    counter = 0;
    unsigned int actuals = avm_totalactuals();
    if (actuals != 1) {
        avm_error(
            "Invalid use of library function member keys. Can only be "
            "called with 1 argument.\n");
        avm_memcellclear(&retval);
        retval.type = nil_m;
        return;
    }
    avm_memcell *argument = avm_getactual(0);
    if (argument->type != table_m) {
        avm_error("Member keys expected a table argument.\n");
        return;
    }
    avm_table *table = argument->data.tableVal;
    retval.type = table_m;
    retval.data.tableVal = avm_tablenew();

    HashMap_apply_all(table->numIndexed, wrapper_insert, noop);
    HashMap_apply_all(table->strIndexed, wrapper_insert, noop);
    HashMap_apply_all(table->boolIndexed, wrapper_insert, noop);
    HashMap_apply_all(table->tableIndexed, wrapper_insert, noop);
    HashMap_apply_all(table->userfuncIndexed, wrapper_insert, noop);
    HashMap_apply_all(table->libfuncIndexed, wrapper_insert, noop);
}


void libfunc_input() {
    char *buff = malloc(256 * sizeof(char));
    scanf("%s", buff);

    if (atof(buff) != 0) {
        retval.type = number_m;
        retval.data.numVal = atof(buff);
    } else if (compare_strings(buff, "true")) {
        retval.type = bool_m;
        retval.data.boolVal = true;
    } else if (compare_strings(buff, "false")) {
        retval.type = bool_m;
        retval.data.boolVal = false;
    } else if (compare_strings(buff, "nil")) {
        avm_memcellclear(&retval);
        retval.type = nil_m;
    } else {
        retval.type = string_m;
        retval.data.strVal = buff;
    }
    return;
}

void libfunc_objecttotalmembers(void) {
    unsigned int n = avm_totalactuals();
    if (n != 1)
        avm_error(
            "Invalid use of library function objecttotalmembers. Can only be "
            "called with 1 argument.\n");
    else {
        avm_memcell *res = avm_getactual(0);
        if (res->type != table_m) {
            avm_error("objecttotalmembers expected a table argument.\n");
        } else {
            retval.type = number_m;
            retval.data.numVal = res->data.tableVal->total;
        }
    }
}
void libfunc_totalarguments(void) {
    unsigned int p_topsp = avm_get_envvalue(
        topsp + AVM_SAVEDTOPSP_OFFSET); /*topsp of prev activation record */
    avm_memcellclear(&retval);

    if (!p_topsp) { /* No previous activation record exists */
        avm_warning("'total arguments' called outside a function!\n");
        retval.type = nil_m;
    } else { /* extract argument count for prev activation record */
        retval.type = number_m;
        retval.data.numVal = avm_get_envvalue(p_topsp + AVM_NUMACTUALS_OFFSET);
    }
}

void libfunc_argument(void) {
    unsigned int p_topsp = avm_get_envvalue(
        topsp + AVM_SAVEDTOPSP_OFFSET); /*topsp of prev activation record */
    unsigned int i = avm_totalactuals();
    if (i != 1) {
        avm_error(
            "Invalid use of library function argument. Can only be called with "
            "1 argument.\n");
        retval.type = nil_m;
        return;
    }
    avm_memcell *res = avm_getactual(0);
    if (res->type != number_m) {
        avm_error(
            "Invalid use of library function argument. Can only be called with "
            "1 number.\n");
        retval.type = nil_m;
        return;
    }
    i = res->data.numVal;
    avm_memcellclear(&retval);

    if (!p_topsp) { /* No previous activation record exists */
        avm_warning("'argument' can't be called outside a function!\n");
        retval.type = nil_m;
    } else { /* extract i-th argument */
        if (i < avm_get_envvalue(p_topsp + AVM_NUMACTUALS_OFFSET))
            retval = stack[p_topsp + AVM_STACKENV_SIZE + 1 + i];
        else {
            avm_warning("Argument no.%d doesn't exist in this function.\n", i);
            retval.type = nil_m;
        }
    }
}

void libfunc_typeof(void) {
    unsigned n = avm_totalactuals();

    if (n != 1)
        avm_error("one argument (not %d) expected in 'typeof'.\n", n);
    else {
        avm_memcellclear(&retval);
        retval.type = string_m;
        retval.data.strVal = strdup(typeStrings[avm_getactual(0)->type]);
    }
}

void libfunc_print(void) {
    unsigned int n = avm_totalactuals();
    for (unsigned int i = 0; i < n; ++i) {
        char *s = avm_tostring(avm_getactual(i));
        fputs(s, stdout);
        free(s);
    }
}

void avm_registerlibfunc(char *id, library_func_t addr) {
    assert(id && addr);
    HashMap_insert(lib_map, id, addr);
    return;
}

library_func_t avm_getlibraryfunc(char *id) {
    assert(id);
    return (library_func_t)HashMap_get(lib_map, id);
}
