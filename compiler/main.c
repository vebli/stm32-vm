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
            if(strlen(line_buffer) <= 1) continue; // ignoring empty lines (brittle)
            char c = ' ';
            int i = 0; int j = 0;
            int word_count= 1;
            line_number++;
            int skip_write = 0;
            Opcode opcode; 
            int arg1;
            int arg2;
            int type1;
            int type2;
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
                    // Comment
                    if (word_buffer[0] == ';') {
                        if(word_count == 1) skip_write = 1;
                        LOG_MSG("Ignoring comment %s\n", (&line_buffer[i] - strlen(word_buffer)));
                        break;
                    }
                    // Check if first word in line -> Opcode
                    if(word_count == 1){ 
                        if(get_opcode(word_buffer, &opcode)){
                            LOG_MSG("Opcode:\t\t%01X <-> %s\n", opcode, word_buffer);
                        }
                        else {
                            fprintf(stderr, "[ERROR]: Invalid opcode '%s'\n -> %d| %s", word_buffer, line_number, line_buffer);
                            goto quit;
                        }
                    }
                    // Register Arguments
                    else if(j > 1 && is_register(word_buffer)){ 
                        const int argument_num = word_count - 1;
                        LOG_MSG("Argument %d:\t%s (Register)\n", argument_num, word_buffer);
                        if(argument_num == 1) { 
                            type1 = REGISTER;
                            arg1 = atoi(&word_buffer[1]);
                        }
                        else if(argument_num == 2) { 
                            type2 = REGISTER;
                            arg2 = atoi(&word_buffer[1]);
                        }
                    }
                    // Immediate Arguments
                    else{ 
                        const int argument_num = word_count - 1;
                        LOG_MSG("Argument %d:\t%s (Immediate)\n", argument_num, word_buffer);
                        if(argument_num == 1) { 
                            type1 = IMMEDIATE;
                            arg1 = atoi(word_buffer);
                        }
                        else if(argument_num == 2) { 
                            type2 = IMMEDIATE;
                            arg2 = atoi(word_buffer);
                        }
                    }
                    j = 0;
                    word_count++;
                }
                else {
                    word_buffer[j++] = c;
                }
                i++; 
            }
            if(!skip_write){
                if(opcode == OP_LBI){
                    type1 = REGISTER;
                    type2 = IMMEDIATE;
                    uint16_t instr_word = ENCODE_INSTR(opcode, type1, arg1, type2, 0);
                    fwrite(&instr_word, sizeof(instr_word), 1, wf);
                    fwrite(&arg2, sizeof(uint16_t), 1, wf);
                    LOG_MSG("Wrote instruction: %04X %04X <-> %016b %016b\n ", instr_word, (uint16_t) arg2, instr_word, (uint16_t) arg2);
                }
                else{
                    LOG_MSG("Words found:\t%d\n", word_count-1);
                    uint16_t instr_word = ENCODE_INSTR(opcode, type1, arg1, type2, arg2);
                    LOG_MSG("Wrote instruction: %04X <-> %016b\n", instr_word, instr_word);
                    fwrite(&instr_word, sizeof(instr_word), 1, wf);
                }
            }
        }
quit:
        fclose(rf);
        fclose(wf);
    }
}
