#ifndef OPCODES
#define OPCODES


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

const OpcodeEntry opcode_table[] = {
#define X(name) { #name, OP_##name },
    OPCODE_LIST
#undef X
};

int get_opcode(const char *name);

#endif //OPCODES
