#ifndef AVM_EXEC_TABLES_H
#define AVM_EXEC_TABLES_H

#include "../memory/instruction.h"
#include "../memory/memory.h"

void execute_newtable(Instruction *);
void execute_tablegetelem(Instruction *);
void execute_tablesetelem(Instruction *);

#endif  // AVM_EXEC_TABLES_H
