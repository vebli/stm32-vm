#include "opcodes.h"
#include <stdio.h>
#include <string.h>

const char *program_name = NULL;

void usage(void){
    printf("Usage: %s [input_file] -o [output_file]", program_name);
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

        const size_t buffer_size = 128;
        char buffer[buffer_size]; 
        unsigned int opcode;

        if(!rf) {
            printf("Error opening file %s", rfile_name);
            return 1;
        }

        if(!wf) {
            printf("Error opening file %s", wfile_name);
            return 1;
        }

        while (fgets(buffer, sizeof(buffer), rf)) {

            // remove newline
            buffer[strcspn(buffer, "\r\n")] = '\0';

            if(!get_opcode(buffer, &opcode)){
                fprintf(stderr, "Found undefined opcode %0*d",OP_LENGTH, opcode);
                return 1;
            }
            if(!fprintf(wf, "%0*d", OP_LENGTH, opcode)){
                fprintf(stderr,"Error writing to file %s", wfile_name);
                return 1;
            }
        }

        if(ferror(rf)){
            fprintf(stderr, "Error reading from file %s", rfile_name);
        }

        fclose(rf);
        fclose(wf);
    }
}


