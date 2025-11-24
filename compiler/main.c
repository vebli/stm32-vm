#include "opcodes.h"
#include "sys/types.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ENABLE_DEBUG 1

#if ENABLE_DEBUG
    #define LOG_MSG(fmt, ...) printf("[LOG] " fmt , ##__VA_ARGS__)
#else 
    #define LOG_MSG(...)
#endif

const char *program_name = NULL;

void usage(void){
    LOG_MSG("Usage: %s [input_file] -o [output_file]", program_name);
}

//PRE: string length > 1
int is_register(const char *str){
    return (str[0] == 'R' && atoi(&str[1]) < NUM_REGISTERS) ? 1 : 0;
}


int main(const int argc, const char* argv[]){
    program_name = argv[0];
    if(argc == 1){ 
        usage();
        return 0;
    };

    int curr_arg = 1;
    const char *rfile_name = argv[curr_arg++];
    const char *wfile_name = "a.out";

    while(curr_arg < argc){
        if(strcmp(argv[curr_arg++], "-o") == 0){
            if(curr_arg >= argc){
                usage(); 
                break;
            }
            wfile_name = argv[curr_arg++];
        }
        else {
            usage(); 
            return 1; 
        }
    }

    if(rfile_name){
        FILE *rf = fopen(rfile_name, "r");
        FILE *wf = fopen(wfile_name, "w");

        const size_t line_buffer_size = 128;
        char line_buffer[line_buffer_size]; 
        const size_t word_buffer_size = 8;
        char word_buffer[word_buffer_size];
        unsigned int opcode;
        unsigned int line_number = 0;

        if(!rf) {
            LOG_MSG("Error opening file %s", rfile_name);
            return 1;
        }

        if(!wf) {
            LOG_MSG("Error opening file %s", wfile_name);
            return 1;
        }

        //line parsing
        while (fgets(line_buffer, sizeof(line_buffer), rf)) {
            LOG_MSG("Parsing line %d:\t%s", line_number, line_buffer);
            
            char c = ' ';
            int i = 0; int j = 0;
            int word_count= 1;
            line_number++;
            Instruction instr = {0};
            if(ferror(rf)){
                fprintf(stderr, "Error reading from file %s", rfile_name);
            }
            //Word parsing
            while(c != '\0' && c != '\n'){
                assert((size_t)i < line_buffer_size);
                c = line_buffer[i]; 
                if(c == ' ' || c == '\n' || c == '\0'){
                    assert((size_t)j < word_buffer_size);
                    word_buffer[j] = '\0';
                    // Check if first word in line -> Opcode
                    if(word_count == 1){ 
                        if(get_opcode(word_buffer, &opcode)){
                            LOG_MSG("Opcode:\t\t%0*b <-> %s\n", OP_LENGTH, opcode, word_buffer);
                            instr.opcode = opcode;
                        }
                        else {
                            fprintf(stderr, "[ERROR]: Invalid opcode '%s'\n -> %d| %s", word_buffer, line_number, line_buffer);
                            goto quit;
                        }
                    }
                    // Register Arguments
                    else if(j > 1 && is_register(word_buffer)){ 
                        LOG_MSG("Argument %d:\t%s (Register)\n", word_count - 1, word_buffer);
                        const int argument_num = word_count - 2;
                        instr.arg_type[argument_num]= REGISTER;
                        instr.arg[argument_num] = atoi(&word_buffer[1]);
                    }
                    // Immediate Arguments
                    else{ 
                        LOG_MSG("Argument %d:\t%s (Immediate)\n", word_count - 1, word_buffer);
                        const int argument_num = word_count - 2;
                        instr.arg_type[argument_num]= IMMEDIATE;
                        instr.arg[argument_num] = atoi(word_buffer);
                    }
                    j = 0;
                    word_count++;
                }
                else {
                    word_buffer[j++] = c;
                }
                i++; 
            }
            LOG_MSG("Words found:\t%d\n", word_count-1);
            uint16_t instr_word = encode_instruction(&instr);
            LOG_MSG("Instruction:\t%016b <-> %04X\n",  instr_word, instr_word);
            fwrite(&instr_word, sizeof(instr_word), 1, wf);
        }

quit:
        fclose(rf);
        fclose(wf);
    }
}
