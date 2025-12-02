#ifndef VM_H
#define VM_H

#include "opcodes.h"

#define PROGRAM_SIZE_BYTES (INSTRUCTION_LENGTH * 128)
#define PROGRAM_SIZE_WORDS (PROGRAM_SIZE_BYTES / 2)

extern uint8_t vm_enable_logs; 
extern uint16_t program[PROGRAM_SIZE_WORDS];
extern uint16_t pc;
extern uint32_t reg[NUM_REGISTERS];

typedef struct {
    unsigned int run_program : 1;
    unsigned int print_reg : 1;
    unsigned int step : 1;
    unsigned int print_program : 1;
    unsigned int print_stack : 1;
    unsigned int print_prompt : 1;
    unsigned int load : 1;
}vm_flags_t ;

extern vm_flags_t vm_flags;

typedef struct {
    uint16_t x;
    uint16_t y;
} pixel;

#define PIX_STACK_MAX 4096
extern pixel pix_stack[PIX_STACK_MAX];
extern int pix_sp;


void vm_init();

int vm_run_instruction();
// void vm_run_program();

void vm_run_repl(void);
void vm_draw(void);
void vm_draw_line(void);
void vm_draw_rect(void);

pixel vm_push_pix(uint16_t x, uint16_t y);
pixel vm_pop_pix(void);

#endif 
