#ifndef OPCODES_H
#define OPCODES_H

#include <stdint.h>

#define OPCODE(instr)         (((instr) >> 12) & 0xF)

#define ARG1_TYPE(instr)      (((instr) >> 11) & 0x1)
#define ARG1_VALUE(instr)     (((instr) >> 6)  & 0x1F)

#define ARG2_TYPE(instr)      (((instr) >> 5)  & 0x1)
#define ARG2_VALUE(instr)     ((instr)         & 0x1F)

#define ENCODE_INSTR(op, at0, a0, at1, a1) \
    ( ((uint16_t)(op)  & 0xF)  << 12 | \
      ((uint16_t)(at0) & 0x1)  << 11 | \
      ((uint16_t)(a0)  & 0x1F) << 6  | \
      ((uint16_t)(at1) & 0x1)  << 5  | \
      ((uint16_t)(a1)  & 0x1F) )

#define OPCODE_LIST\
    X(ADD)\
    X(SUB)\
    X(MLT)\
    X(JMP)\
    X(JZ)\
    X(MOV)\
    X(LBI)\
    X(BTN)\
    X(PIX)\
    X(RECT)\
    X(LINE)\
    X(CLS)\
    X(DRAW)\
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
#define OP_HEX_LENGTH 1
#define OP_BIT_LENGTH OP_HEX_LENGTH * 4
#define OPERAND_LENGTH 5
#define NUM_REGISTERS 16
#define MAX_NUM_ARGUMENTS 2
#define INSTRUCTION_LENGTH 16

// typedef struct{
//     uint8_t opcode;
//     uint8_t arg[MAX_NUM_ARGUMENTS];
//     uint8_t arg_type[MAX_NUM_ARGUMENTS];
// } Instruction; 

typedef enum {
    REGISTER,
    IMMEDIATE,
    BIG_IMMEDIATE,
} OperandType;


typedef struct {
    const char *name;
    Opcode opcode;
} OpcodeEntry;

extern const OpcodeEntry opcode_table[];

int get_opcode(const char *name, Opcode *opcode_ptr);
// uint16_t encode_instruction(const Instruction *instr);
// Instruction decode_instruction(uint16_t instr);

#endif 
