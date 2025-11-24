#ifndef OPCODES_H
#define OPCODES_H

#include <stdint.h>

#define OPCODE(instr)         (((instr) >> 12) & 0xF)

#define ARG0_TYPE(instr)      (((instr) >> 11) & 0x1)
#define ARG0_VALUE(instr)     (((instr) >> 6)  & 0x1F)

#define ARG1_TYPE(instr)      (((instr) >> 5)  & 0x1)
#define ARG1_VALUE(instr)     ((instr)         & 0x1F)

#define OPCODE_LIST\
    X(ADD)\
    X(MLT)\
    X(JMP)\
    X(MOV)\
    X(BTN)\
    X(PIX)\
    X(CLS)\
    X(DRAW)\
    X(BTN0)\
    X(BTN1)\
    X(JOY)\
    X(WAIT)\
    X(HALT)\

typedef enum {
#define X(name) OP_##name,
    OPCODE_LIST
#undef X 
    NUM_OPCODES
} Opcode;

//[opcode:4][arg1_type:1][arg1_value:5][arg2_type:1][arg2_value:5] = 16bit

#define TYPE_LENGTH 1
#define OP_LENGTH 4
#define OPERAND_LENGTH 5
#define NUM_REGISTERS (1 << 3)
#define MAX_NUM_ARGUMENTS 2
#define INSTRUCTION_LENGTH 16

typedef struct{
    uint8_t opcode;
    uint8_t arg[MAX_NUM_ARGUMENTS];
    uint8_t arg_type[MAX_NUM_ARGUMENTS];
} Instruction; 

typedef enum {
    IMMEDIATE,
    REGISTER
} OperandType;


typedef struct {
    const char *name;
    Opcode opcode;
} OpcodeEntry;

extern const OpcodeEntry opcode_table[];

int get_opcode(const char *name, unsigned int *opcode_ptr);
void print_instruction(const Instruction * instr);
uint16_t encode_instruction(const Instruction *instr);
Instruction decode_instruction(uint16_t instr);

#endif 
