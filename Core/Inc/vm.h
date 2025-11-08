#ifndef VM_H
#define VM_H

#include "opcodes.h"

#define PROGRAM_SIZE_BYTES (INSTRUCTION_LENGTH * 128)

extern uint16_t program[PROGRAM_SIZE_BYTES];
// extern uint16_t reg[NUM_REGISTERS];
// extern uint16_t pc[NUM_REGISTERS];

void vm_init();

void vm_print_state();

int get_instruction(uint16_t *pc, uint16_t *instr_buffer);

void run_program(uint16_t *pc);

#endif 
