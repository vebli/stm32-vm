#ifndef OPCODES_H
#define OPCODES_H

#define OP_LENGTH 4

#define OPCODE_LIST\
    X(GPIO)\
    X(OP1)\
    X(OP2)\
    X(OP3)\
    X(OP4)

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
