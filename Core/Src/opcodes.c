#include "opcodes.h"
#include <string.h>
#include <stdio.h>

const OpcodeEntry opcode_table[] = {
#define X(name) { #name, OP_##name },
    OPCODE_LIST
#undef X
};

int get_opcode(const char* name, Opcode *opcode_ptr){
    for(int i = 0; i < NUM_OPCODES; i++){
        if(strcmp(opcode_table[i].name, name) == 0){
            *opcode_ptr = opcode_table[i].opcode;
            return 1;
        };
    }
    return 0;
}

