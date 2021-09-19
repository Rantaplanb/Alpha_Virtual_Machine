#ifndef AVM_EXEC_FUNCTIONS_H
#define AVM_EXEC_FUNCTIONS_H
#include "../memory/memory.h"

library_func_t avm_getlibraryfunc(char *id);

void execute_call(Instruction *);
void execute_pusharg(Instruction *);
void execute_funcenter(Instruction *);
void execute_funcexit(Instruction *);


#endif
