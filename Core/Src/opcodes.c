#include "opcodes.h"
#include <string.h>

int get_opcode(const char* name){
    for(int i = 0; i < OP_COUNT; i++){
        if(strcmp(opcode_table[i].name, name) == 1){
            return opcode_table[i].opcode;
        };
        return 0;
    }
}
