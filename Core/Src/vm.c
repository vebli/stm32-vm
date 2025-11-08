#include "vm.h"
#include "opcodes.h"
#include "printf_uart.h"

uint16_t program[PROGRAM_SIZE_BYTES];
uint16_t *pc = program;
uint16_t reg[NUM_REGISTERS];

void vm_init(){
    for(int i = 0; i < NUM_REGISTERS; i++){
        reg[i] = 0;
    }
}

int get_instruction(uint16_t *pc, uint16_t *instr_buffer){
    const uint16_t instr = pc[0];
    if(instr == OP_HALT) return 1;

    *instr_buffer = instr;
    pc += sizeof(uint16_t);
    return 0;
}

void vm_print_state(){
    printf("Registers:\r\n\t");
    for (int i = 0; i < NUM_REGISTERS; i++){
        printf("R%d: %05d\t", i, reg[i]);
    }
    printf("\r\n");
    printf("\tpc: %d\r\n", pc-program);
}

//TODO: Error handling
void vm_run_program(uint16_t *pc){

    Instruction instr;
    uint16_t word;
    while(get_instruction(pc, &word)){
        decode_instruction(word, &instr);
        for(int i = 0; i < MAX_NUM_ARGUMENTS; i++){
            if(instr.arg_type[i] == REGISTER) {
                instr.arg[i] = reg[instr.arg[i]];
            }
        }
        switch(instr.opcode){
            case OP_ADD: 
                reg[0] = instr.arg[0] + instr.arg[1]; 
                break;

            case OP_MLT: 
                reg[0] = instr.arg[0] * instr.arg[1]; 
                break;

            case OP_MOV: 
                reg[instr.arg[1]] = reg[instr.arg[0]];
                break;

            case OP_JMP:
                pc = &program[instr.arg[1]];
                break;

            // case OP_BTN:
            //     break;
        };
    }
}
