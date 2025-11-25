#include "vm.h"
#include "opcodes.h"
#include "printf_uart.h"
#include "hardware.h"
#include "stm32l4xx_hal.h"
#include <stdlib.h>


uint16_t program[PROGRAM_SIZE_WORDS];
uint16_t pc = 0;

uint16_t reg[NUM_REGISTERS];
uint8_t vm_enable_logs = 0;

uint8_t frame_buffer[12000];

void vm_init(){
    for(int i = 0; i < NUM_REGISTERS; i++){
        reg[i] = 0;
    }
}



//TODO: Error handling
int vm_run_instruction(){
    joystick_direction joystick_direction = joystick_read();
    uint8_t pressed_button0 = button_read(0);
    uint8_t pressed_button1 = button_read(1);   
    uint16_t instr= program[pc++];
    uint8_t opcode = OPCODE(instr);
    uint8_t arg1 = ARG1_VALUE(instr);
    uint8_t arg2 = ARG2_VALUE(instr);
    uint8_t type1 = ARG1_TYPE(instr);
    uint8_t type2 = ARG2_TYPE(instr);
    

    if(vm_enable_logs){
        if(pressed_button0){
            printf("Pressed Button 0\r\n");
        }
        if(pressed_button1){
            printf("Pressed Button 1\r\n");
        }
        printf("direction: %d\r\n", joystick_direction);
    }

    printf("\t%d, %d, %d\r\n", opcode, arg1, arg2);

    switch(opcode){
        case OP_ADD: 
            if(type1 == REGISTER && arg1 < NUM_REGISTERS) { arg1 = reg[arg1]; }
            if(type2 == REGISTER && arg2 < NUM_REGISTERS) { arg2 = reg[arg2]; }
            reg[0] = arg1 + arg2; 
            break;

        case OP_MLT: 
            if(type1 == REGISTER && arg1 < NUM_REGISTERS) { arg1 = reg[arg1]; }
            if(type2 == REGISTER && arg2 < NUM_REGISTERS) { arg2 = reg[arg2]; }
            reg[0] = arg1 * arg2; 
            break;

        case OP_MOV: 
            if(type2 == REGISTER && arg2 < NUM_REGISTERS) { arg2 = reg[arg2]; }
            if(arg1 < NUM_REGISTERS){
                reg[arg1] = arg2;
            }
            break;

        case OP_LBI:
            if(arg1 < NUM_REGISTERS){
                reg[arg1] = program[pc++];
            }
            break;

        case OP_JMP:
            pc = arg1;
            break;

        case OP_PIX:
            vm_push_pix(arg1, arg2);
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
            HAL_Delay(arg1);
            break;
        case OP_HALT:
            return 0;
            break;
    }
    return 1;
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


