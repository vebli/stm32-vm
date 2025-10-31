#include "opcodes.h"
#include <string.h>

const OpcodeEntry opcode_table[] = {
#define X(name) { #name, OP_##name },
    OPCODE_LIST
#undef X
};

int get_opcode(const char* name, unsigned int *opcode_ptr){
    for(int i = 0; i < OP_COUNT; i++){
        if(!strcmp(opcode_table[i].name, name)){
            return 1;
        };
        *opcode_ptr = opcode_table[i].opcode;
    }
    return 0;
}
