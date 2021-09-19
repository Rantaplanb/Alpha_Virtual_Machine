#ifndef AVM_FUNCTION_H
#define AVM_FUNCTION_H

typedef struct {
    unsigned address;
    unsigned local_size;
    char *id;
} UserFunc;

#endif  // AVM_FUNCTION_H
