#ifndef AVM_EXEC_BRANCHES_H
#define AVM_EXEC_BRANCHES_H
#include <stdbool.h>

#include "../memory/instruction.h"

bool JGE_impl(double x, double y);
bool JGT_impl(double x, double y);
bool JLE_impl(double x, double y);
bool JLT_impl(double x, double y);

void execute_jeq(Instruction *instr);
void execute_jne(Instruction *instr);
void execute_jle(Instruction *instr);
void execute_jge(Instruction *instr);
void execute_jlt(Instruction *instr);
void execute_jgt(Instruction *instr);
void execute_jump(Instruction *instr);
void comparison_dispatcher(Instruction *instr);

#endif  // AVM_EXEC_BRANCHES_H
