#ifndef VM_H
#define VM_H

#include "opcodes.h"

#define PROGRAM_SIZE_BYTES (INSTRUCTION_LENGTH * 128)
#define PROGRAM_SIZE_WORDS (PROGRAM_SIZE_BYTES / 2)

extern uint8_t vm_enable_logs; 
extern uint16_t program[PROGRAM_SIZE_WORDS];
// extern uint16_t reg[NUM_REGISTERS];
// extern uint16_t pc[NUM_REGISTERS];

typedef struct {
    uint8_t x;
    uint8_t y;
} pixel;

void vm_init();

void vm_print_state();

int vm_next_instruction(uint16_t *instr_buffer);

void vm_run_instructon(uint16_t* word);
// void vm_run_program();

void vm_run_repl(void);
void vm_draw(void);

pixel vm_push_pix(uint16_t x, uint16_t y);
pixel vm_pop_pix(void);

#endif 
