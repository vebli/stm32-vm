#include <stdio.h>
#include "opcodes.h"

void usage(void){
    printf("Expected path");
}

int main(int argc, const char **argv){
    if(argc == 3){
        const char *input_path = argv[1];
        const char *output_path = argv[2];
        FILE *input_file = fopen(input_path, "r");
        FILE* output_file = fopen("output", "w");
        char input_buffer[256];
        char output_buffer[256];
        if(input_file) {
            while(fgets(input_buffer, sizeof(input_buffer), input_file)){
                //TODO: Extract string from buffer.
                // size_t n = 0;
                // fprintf(output_file, "%04d", input_buffer);
            }
        }
        fclose(input_file);
        fclose(output_file);

    }    
    else {
        usage();
        return 1;
    }
}

