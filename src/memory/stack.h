#ifndef AVM_STACK_H
#define AVM_STACK_H

#include "memory.h"

#ifndef AVM_STACKENV_SIZE
#define AVM_STACKENV_SIZE 4
#endif

#ifndef AVM_STACKSIZE
#define AVM_NUMACTUALS_OFFSET 4
#define AVM_SAVEDPC_OFFSET 3
#define AVM_SAVEDTOP_OFFSET 2
#define AVM_SAVEDTOPSP_OFFSET 1
#define AVM_STACKSIZE 4096
#define AVM_WIPEOUT(m) memset(&(m), 0, sizeof(m))
#endif

extern avm_memcell stack[AVM_STACKSIZE];
extern unsigned current_stack_size;
extern unsigned top, topsp;
extern unsigned totalActuals;


void avm_initialize(void);

void avm_dec_top(void);

void avm_push_envvalue(unsigned int val);

unsigned avm_get_envvalue(unsigned i);

void avm_callsaveenvironment(void);

void avm_calllibfunc(char *func_name);

unsigned int avm_totalactuals(void);

avm_memcell *avm_getactual(unsigned int i);


#endif  // AVM_STACK_H
