#include "opcodes.h"
#include <string.h>
#include <stdio.h>

const OpcodeEntry opcode_table[] = {
#define X(name) { #name, OP_##name },
    OPCODE_LIST
#undef X
};

int get_opcode(const char* name, unsigned int *opcode_ptr){
    for(int i = 0; i < NUM_OPCODES; i++){
        if(strcmp(opcode_table[i].name, name) == 0){
            *opcode_ptr = opcode_table[i].opcode;
            return 1;
        };
    }
    return 0;
}

void print_instruction(const Instruction* instr_ptr){
    printf("Opcode:\t%0*d\n", OP_LENGTH, instr_ptr->opcode);
    const char *str = "Argument %d:\n\
    Type: %0*d\n\
    Value: %0*d\n";

    for(size_t i = 0; i < instr_ptr->args_size; i++){
        const Arg *arg = &instr_ptr->args[i];
        printf(str, i+1, TYPE_LENGTH, arg->type, TYPE_LENGTH, arg->value);
    }
}

