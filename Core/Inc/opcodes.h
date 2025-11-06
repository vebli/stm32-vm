#ifndef OPCODES_H
#define OPCODES_H

#include <stddef.h>
#include <stdint.h>

#define OPCODE_LIST\
    X(add)\
    X(sub)\
    X(jmp)\
    X(btn)\
    X(pix)\
    X(cls)\
    X(halt)

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
#define INSTRUCTION_LENGTH (OP_LENGTH + MAX_NUM_ARGUMENTS * (OPERAND_LENGTH + TYPE_LENGTH))

typedef enum {
    IMMEDIATE,
    REGISTER
} OperandType;

typedef struct {
    OperandType type;
    int value;
} Arg; 


typedef struct{
    Opcode opcode;
    Arg args[MAX_NUM_ARGUMENTS];
    size_t args_size;
} Instruction; 

typedef struct {
    const char *name;
    Opcode opcode;
} OpcodeEntry;

extern const OpcodeEntry opcode_table[];

int get_opcode(const char *name, unsigned int *opcode_ptr);
void print_instruction(const Instruction* instr);
// void encode_instruction(const Instruction *instr, char *instr_buffer, size_t buffer_size);
void decode_instruction(void);
#endif 
