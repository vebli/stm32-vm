#include "opcodes.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *program_name = NULL;

void usage(void){
    printf("Usage: %s [input_file] -o [output_file]", program_name);
}

//PRE: string length > 1
int is_register(const char *str){
    return (str[0] == 'R' && atoi(&str[1]) <= NUM_REGISTERS) ? 0 : 1;
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
        const size_t word_buffer_size = 4;
        char word_buffer[word_buffer_size];
        unsigned int opcode;
        unsigned int line_number = 1;

        if(!rf) {
            printf("Error opening file %s", rfile_name);
            return 1;
        }

        if(!wf) {
            printf("Error opening file %s", wfile_name);
            return 1;
        }

        while (fgets(line_buffer, sizeof(line_buffer), rf)) {

            printf("\nEvaluating line %d:\t %s", line_number++, line_buffer);
            
            char c = ' ';
            int i = 0; int j = 0;
            int word_count= 1;
            /*
            FORMAT: [opcode][arg1_type][arg1_value][arg2_type][arg2_value]
            types:
            0 -> Immediate
            1 -> Register
            */
            while(c != '\0' && c != '\n'){
                assert(i < line_buffer_size);
                c = line_buffer[i]; 
                if(c == ' ' || c == '\n' || c == '\0'){
                    assert(j < word_buffer_size);
                    word_buffer[j] = '\0';
                    // Check opcode if first word in line -> Opcode
                    if(word_count == 1){ 
                        if(get_opcode(word_buffer, &opcode)){
                            printf("Found instruction:\t %0*d <-> %s\n", OP_LENGTH, opcode, word_buffer);
                        }
                        else {
                            fprintf(stderr, "Invalid opcode:\t %0*d\n",OP_LENGTH, opcode);
                        }
                    }
                    // Register Arguments
                    else if(j > 1 && is_register(word_buffer) == 0){ 
                        printf("Argument %d:\t\t %s (Register)\n", word_count - 1, word_buffer);

                    }
                    // Immediate Arguments
                    else{ 
                        printf("Argument %d:\t\t %s (Immediate)\n", word_count - 1, word_buffer);
                    }
                    j = 0;
                    word_count++;
                }
                else {
                    word_buffer[j++] = c;
                }
                i++; 
            }
        }

        if(ferror(rf)){
            fprintf(stderr, "Error reading from file %s", rfile_name);
        }

        fclose(rf);
        fclose(wf);
    }
}


