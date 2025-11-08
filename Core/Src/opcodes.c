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

    for(size_t i = 0; i < MAX_NUM_ARGUMENTS; i++){
        printf(str, i+1, TYPE_LENGTH, &instr_ptr->arg_type[i], TYPE_LENGTH, &instr_ptr->arg[i]);
    }
}

void decode_instruction(uint16_t instr, Instruction* decoded_instr){
        decoded_instr->arg_type[0] = (instr >> 11) & 0x1;
        decoded_instr->arg[0] = (instr >> 6) & 0x1F;
        decoded_instr->arg_type[1] = (instr >> 5) & 0x1;
        decoded_instr->arg[1] = instr & 0x1F;
}
