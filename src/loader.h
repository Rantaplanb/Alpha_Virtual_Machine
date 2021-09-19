#ifndef AVM_LOADER_H
#define AVM_LOADER_H

#include <stdio.h>

#include "memory/function.h"
#include "memory/instruction.h"

/*
 * Structure of Alpha byte code executables (.abc files):
 *
 * -----------------------------------------------------------------------------
 * | magic number (unsigned long long abc file identifier)                     |
 * |                                                                           |
 * | const strings number (unsigned)                                           |
 * | const string #1 (null terminated)                                         |
 * | const string #2 (null terminated)                                         |
 * | ...                                                                       |
 * |                                                                           |
 * | const numbers number (unsigned)                                           |
 * | const number #1 (double)                                                  |
 * | const number #2 (double)                                                  |
 * | ...                                                                       |
 * |                                                                           |
 * | user functions number (unsigned)                                          |
 * | user function #1 (4b address - 4b localsize - null terminated id)         |
 * | user function #2 (4b address - 4b localsize - null terminated id)         |
 * | ...                                                                       |
 * |                                                                           |
 * | library functions used number (unsigned)                                  |
 * | library function #1 (null terminated id)                                  |
 * | library function #2 (null terminated id)                                  |
 * | ...                                                                       |
 * |                                                                           |
 * | instructions number (unsigned)                                            |
 * | instruction #1 src line (unsigned)                                        |
 * | instruction #2 src line (unsigned)                                        |
 * | ...                                                                       |
 * |                                                                           |
 * | instruction #1 (opcode (1b) - operand (1b type - unsigned value)x3)       |
 * | instruction #2 (opcode (1b) - operand (1b type - unsigned value)x3)       |
 * | ...                                                                       |
 * |___________________________________________________________________________|
 */

void load_abc(FILE *abc_file, char **string_consts[], double *num_consts[],
              UserFunc *funcs_used[], char **lib_funcs_used[],
              Instruction *code[]);

#endif  // AVM_LOADER_H
