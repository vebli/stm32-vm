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
    printf("Opcode: %X\r\n", instr_ptr->opcode);

    for (size_t i = 0; i < MAX_NUM_ARGUMENTS; i++){
        printf("Argument %d:\r\n", (int)(i + 1));
        printf("\tType: %d\r\n", instr_ptr->arg_type[i]);
        printf("\tValue: %d\r\n", instr_ptr->arg[i]);
    }
}

Instruction decode_instruction(uint16_t instr){
        Instruction decoded_instr = {0};
        decoded_instr.opcode      = (instr >> 12) & 0xF;
        decoded_instr.arg_type[0] = (instr >> 11) & 0x1;
        decoded_instr.arg[0] = (instr >> 6) & 0x1F;
        decoded_instr.arg_type[1] = (instr >> 5) & 0x1;
        decoded_instr.arg[1] = instr & 0x1F;
        return decoded_instr;
}

uint16_t encode_instruction(const Instruction *instr){
    uint16_t result = 0;
    result |= (instr->opcode & 0xF) << 12;
    result |= (instr->arg_type[0] & 0x1) << 11;
    result |= (instr->arg[0] & 0x1F) << 6;
    result |= (instr->arg_type[1] & 0x1) << 5;
    result |= (instr->arg[1] & 0x1F);
    return result;
}
