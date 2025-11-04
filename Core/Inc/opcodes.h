#ifndef OPCODES_H
#define OPCODES_H

#define OP_LENGTH 4
#define NUM_REGISTERS 8

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
    OP_COUNT
} Opcode;

typedef struct {
    const char *name;
    Opcode opcode;
} OpcodeEntry;

extern const OpcodeEntry opcode_table[];

int get_opcode(const char *name, unsigned int *opcode_ptr);

#endif 
