#ifndef AVM_LIBRARY_FUNCTIONS_H
#define AVM_LIBRARY_FUNCTIONS_H

#include "../memory/memory.h"

void libfunc_cos(void);
void libfunc_sin(void);
void libfunc_sqrt(void);
void libfunc_strtonum(void);
void libfunc_objectcopy(void);
void libfunc_objectmemberkeys(void);
void libfunc_input(void);
void libfunc_totalarguments(void);
void libfunc_argument(void);
void libfunc_typeof(void);
void libfunc_objecttotalmembers(void);
library_func_t avm_getlibraryfunc(char *id);
void libfunc_print(void);
void avm_registerlibfunc(char *id, library_func_t addr);


#endif  // AVM_LIBRARY_FUNCTIONS_H
