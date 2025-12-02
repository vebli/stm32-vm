#include "vm.h"
#include "opcodes.h"
#include "printf_uart.h"
#include "hardware.h"
#include "stm32l4xx_hal.h"
#include "string.h"
#include "usart.h"
#include <stdlib.h>


uint16_t program[PROGRAM_SIZE_WORDS];
uint16_t pc = 0;

uint32_t reg[NUM_REGISTERS];
uint8_t vm_enable_logs = 0;

uint8_t frame_buffer[LCD_FRAME_BUFFER_SIZE];

vm_flags_t vm_flags = {0};

#define PIX_STACK_MAX 4096
pixel pix_stack[PIX_STACK_MAX];
int pix_sp = 0;

void vm_init(){
    lcd_clear();
    for(int i = 0; i < LCD_FRAME_BUFFER_SIZE; i++){
        frame_buffer[i] = 0xFF;
    }
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
    int arg1 = ARG1_VALUE(instr);
    int arg2 = ARG2_VALUE(instr);
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
        printf("\t%d, %d, %d\r\n", opcode, arg1, arg2);
    }


    switch(opcode){
        case OP_ADD: 
            if(type1 == REGISTER && arg1 < NUM_REGISTERS) { arg1 = reg[arg1]; }
            if(type2 == REGISTER && arg2 < NUM_REGISTERS) { arg2 = reg[arg2]; }
            reg[0] = arg1 + arg2; 
            break;

        case OP_SUB: 
            if(type1 == REGISTER && arg1 < NUM_REGISTERS) { arg1 = reg[arg1]; }
            if(type2 == REGISTER && arg2 < NUM_REGISTERS) { arg2 = reg[arg2]; }
            reg[0] = arg1 - arg2; 
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
            arg1 = program[pc++];
            pc = arg1;
            break;

        case OP_JZ:
            arg1 = program[pc++];
            if(reg[0] == 0){
                pc = arg1;
            }
            break;

        case OP_PIX:
            if(type1 == REGISTER && arg1 < NUM_REGISTERS) { arg1 = reg[arg1]; }
            if(type2 == REGISTER && arg2 < NUM_REGISTERS) { arg2 = reg[arg2]; }
            vm_push_pix(arg1, arg2);
            break;

        case OP_RECT: 
            vm_draw_rect();
            break;

        case OP_DRAW:
            vm_draw();
            break;

        case OP_LINE:
            vm_draw_line();
            break;

        case OP_BTN:
            if(type1 == REGISTER && arg1 < NUM_REGISTERS) { arg1 = reg[arg1]; }
            if(arg2 < 2) reg[arg1] = button_read(arg2);
            break;

        case OP_JOY:
            reg[arg1] = joystick_read();
            break;

        case OP_CLS:
            for(int i = 0; i < LCD_FRAME_BUFFER_SIZE; i++ ){
                frame_buffer[i] = 0xFF;
            }
            lcd_clear();
            break;

        case OP_WAIT:
            if(type1 == REGISTER && arg1 < NUM_REGISTERS) { arg1 = reg[arg1]; }
            HAL_Delay(arg1);
            break;
        case OP_HALT:
            return 0;
            break;
    }
    return 1;
}

void vm_draw(void){
    lcd_draw(frame_buffer);
}


void vm_draw_pixel(uint16_t x, uint16_t y, uint8_t color){
    uint16_t index = y*LCD_BYTE_WIDTH + x/8;
    uint8_t mask = 0x80 >> (x % 8);
    if(color == 1){ // white 
        frame_buffer[index] |= mask;
    }
    else { //black
        frame_buffer[index] &= ~mask;
    }
}

void vm_draw_line(void){
    if (pix_sp < 2) return;        
    pixel p2 = vm_pop_pix();   
    pixel p1 = vm_pop_pix();    

    int dx = abs(p2.x - p1.x), sx = p1.x < p2.x ? 1 : -1;
    int dy = -abs(p2.y - p1.y), sy = p1.y < p2.y ? 1 : -1;
    int err = dx + dy, e2;
    
    for(;;) {
        vm_draw_pixel(p1.x, p1.y, 0);  
        if (p1.x == p2.x && p1.y == p2.y) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; p1.x += sx; }
        if (e2 <= dx) { err += dx; p1.y += sy; }
    }
}
void vm_draw_rect(void){
    if (pix_sp < 2) return;        

    pixel p2 = vm_pop_pix();   
    pixel p1 = vm_pop_pix();    

    uint16_t x1 = p1.x;
    uint16_t y1 = p1.y;

    uint16_t x2 = p2.x;
    uint16_t y2 = p2.y;

    // Clamp to screen
    if (x1 >= LCD_PIX_WIDTH)  x1 = LCD_PIX_WIDTH - 1;
    if (x2 >= LCD_PIX_WIDTH)  x2 = LCD_PIX_WIDTH - 1;
    if (y1 >= LCD_PIX_HEIGHT) y1 = LCD_PIX_HEIGHT - 1;
    if (y2 >= LCD_PIX_HEIGHT) y2 = LCD_PIX_HEIGHT - 1;

    // Draw horizontal lines
    for (uint16_t x = x1; x <= x2; x++) {
        frame_buffer[y1 * LCD_PIX_WIDTH + x] = 0;
        frame_buffer[y2 * LCD_PIX_WIDTH + x] = 0;
    }

    // Draw vertical lines
    for (uint16_t y = y1; y <= y2; y++) {
        frame_buffer[y * LCD_PIX_WIDTH + x1] = 0;
        frame_buffer[y * LCD_PIX_WIDTH + x2] = 0;
    }

}
// void vm_run_repl(uint16_t *pc){
// }

pixel vm_push_pix(uint16_t x, uint16_t y){
    if(pix_sp < PIX_STACK_MAX){
        pix_stack[pix_sp++] = (pixel){x, y};
    }
} 

pixel vm_pop_pix(void){
    return pix_stack[--pix_sp];
}


