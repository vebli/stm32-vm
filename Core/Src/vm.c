#include "vm.h"
#include "opcodes.h"
#include "printf_uart.h"
#include "hardware.h"
#include "stm32l4xx_hal.h"
#include <stdlib.h>


uint16_t program[PROGRAM_SIZE_WORDS];
uint16_t *pc = program;
uint16_t reg[NUM_REGISTERS];
uint8_t vm_enable_logs = 0;

uint8_t frame_buffer[12000];

void vm_init(){
    for(int i = 0; i < NUM_REGISTERS; i++){
        reg[i] = 0;
    }
}

int vm_next_instruction(uint16_t *instr_buffer){
    const uint16_t instr = pc[0];
    if(instr == OP_HALT) return 0;

    *instr_buffer = instr;
    pc += sizeof(uint16_t);
    return 1;
}

void vm_print_state(){
    printf("Registers:\r\n");
    for (int i = 0; i < NUM_REGISTERS; i++){
        printf("R%d: %05d\t", i, reg[i]);
    }
    printf("\r\n");
    printf("\tpc: %d\r\n", pc-program);


    printf("Current Instruction:\r\n");
    Instruction instr = decode_instruction(pc[0]);
    print_instruction(&instr);

    printf("Next instructions:\r\n");

    for (int i = 0; i < 4; i++){
        printf("%04X ", pc[i]);
    }
    printf("\r\n");

}

//TODO: Error handling
void vm_run_instructon(uint16_t* word){
    joystick_direction joystick_direction = joystick_read();
    uint8_t pressed_button0 = button_read(0);
    uint8_t pressed_button1 = button_read(1);   
    uint8_t opcode = OPCODE(*word);
    uint8_t arg0 = ARG0_VALUE(*word);
    uint8_t arg1 = ARG1_VALUE(*word);
    uint8_t type0 = ARG0_TYPE(*word);
    uint8_t type1 = ARG1_TYPE(*word);

    if(vm_enable_logs){
        if(pressed_button0){
            printf("Pressed Button 0\r\n");
        }
        if(pressed_button1){
            printf("Pressed Button 1\r\n");
        }
        printf("direction: %d\r\n", joystick_direction);
        vm_print_state();
    }

    if(type0 == REGISTER) {
        arg0 = reg[arg0];
    }

    if(type1 == REGISTER) {
        arg1 = reg[arg1];
    }

    printf("%d, %d\r\n", arg0, arg1);
    switch(opcode){
        case OP_ADD: 
            reg[0] = arg0 + arg1; 
            break;

        case OP_MLT: 
            reg[0] = arg0 * arg1; 
            break;

        case OP_MOV: 
            reg[arg1] = reg[arg0];
            break;

        case OP_JMP:
            pc = &program[arg1];
            break;

        case OP_PIX:
            vm_push_pix(arg0, arg1);
            break;

        case OP_DRAW:
            vm_draw();
            break;

        case OP_BTN0:
            reg[0] = button_read(0);
            break;

        case OP_BTN1:
            reg[0] = button_read(1);
            break;

        case OP_JOY:
            reg[0] = joystick_read();
            break;

        case OP_CLS:
            lcd_clear();
            break;

        case OP_WAIT:
            HAL_Delay(arg0);
            break;
    }
}


void vm_draw(){
    for(int i = 0; i < 12000; i++){
        frame_buffer[i] = 0x0F;
    }
    lcd_draw(frame_buffer);
}
// void vm_run_repl(uint16_t *pc){
// }


#define PIX_STACK_MAX 4096
pixel pix_stack[PIX_STACK_MAX];
int pix_stack_top = 0;

pixel vm_push_pix(uint16_t x, uint16_t y){
    if(pix_stack_top < PIX_STACK_MAX){
        pix_stack[pix_stack_top++] = (pixel){x, y};
    }
} 

pixel vm_pop_pix(void){
    return pix_stack[--pix_stack_top];
}


