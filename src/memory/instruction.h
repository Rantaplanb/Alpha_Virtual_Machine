#ifndef AVM_INSTRUCTION_H
#define AVM_INSTRUCTION_H

typedef enum {
    label_a = 0,
    global_a,
    formal_a,
    local_a,
    number_a,
    string_a,
    bool_a,
    nil_a,
    userfunc_a,
    libfunc_a,
    retval_a,
    undefined_a
} VMarg_type;

typedef enum {
    assign_v = 0,
    add_v,
    sub_v,
    mul_v,
    division_v,
    mod_v,
    if_eq_v,
    if_noteq_v,
    if_lesseq_v,
    if_greatereq_v,
    if_less_v,
    if_greater_v,
    jump_v,
    callfunc_v,
    pusharg_v,
    enterfunc_v,
    funcend_v,
    tablecreate_v,
    tablegetelem_v,
    tablesetelem_v,
    nop_v
} VMopcode;

typedef struct {
    VMarg_type type;
    unsigned val;
} VMarg;

typedef struct {
    VMopcode opcode;
    VMarg result;
    VMarg arg1;
    VMarg arg2;
    unsigned srcLine;
} Instruction;

#endif  // AVM_INSTRUCTION_H
