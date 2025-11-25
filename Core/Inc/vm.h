#ifndef VM_H
#define VM_H

#include "opcodes.h"

#define PROGRAM_SIZE_BYTES (INSTRUCTION_LENGTH * 128)
#define PROGRAM_SIZE_WORDS (PROGRAM_SIZE_BYTES / 2)

extern uint8_t vm_enable_logs; 
extern uint16_t program[PROGRAM_SIZE_WORDS];
extern uint16_t pc;
extern uint16_t reg[NUM_REGISTERS];

typedef struct {
    uint8_t x;
    uint8_t y;
} pixel;

void vm_init();

int vm_run_instruction();
// void vm_run_program();

void vm_run_repl(void);
void vm_draw(void);

pixel vm_push_pix(uint16_t x, uint16_t y);
pixel vm_pop_pix(void);

#endif 
