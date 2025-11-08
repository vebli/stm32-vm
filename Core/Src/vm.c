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
void run_program(uint16_t *pc){
    uint16_t instr;
    uint8_t arg_type[2];
    uint8_t arg[2];
    while(get_instruction(pc, &instr)){
        uint8_t opcode = (instr >> 12) & 0xF;
        arg_type[0] = (instr >> 11) & 0x1;
        arg[0] = (instr >> 6) & 0x1F;
        arg_type[1] = (instr >> 5) & 0x1;
        arg[1] = instr & 0x1F;
        for(int i = 0; i < 2; i++){
            if(arg_type[i] == REGISTER) {
                arg[i] = reg[arg[i]];
            }
        }
        switch(opcode){
            case OP_ADD: 
                reg[0] = arg[0] + arg[1]; 
                break;

            case OP_MLT: 
                reg[0] = arg[0] * arg[1]; 
                break;

            case OP_MOV: 
                reg[arg[1]] = reg[arg[0]];
                break;

            case OP_JMP:
                pc = &program[arg[1]];
                break;

            // case OP_BTN:
            //     break;
        };
    }
}
