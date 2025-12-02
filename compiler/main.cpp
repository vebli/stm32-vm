#include <cstdlib>
#include <vector> 
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <map>
extern "C" {
    #include "opcodes.h"
}

#define ENABLE_DEBUG 1

#if ENABLE_DEBUG
    #define LOG_MSG(fmt, ...) printf("[LOG] " fmt , ##__VA_ARGS__)
#else 
    #define LOG_MSG(...)
#endif

struct Instr {
    Opcode opcode;
    int val[MAX_NUM_ARGUMENTS];
    int type[MAX_NUM_ARGUMENTS];
};

void strip_comment(std::string &line){
    auto pos = line.find(';');
    if(pos != std::string::npos){
        line = line.substr(0,pos);
    }
}
void print_lines(std::vector<std::string> &lines){
    for(auto &line : lines){
        std::cout << line << '\n';
    }
}

const char *program_name = NULL;

void usage(void){
    std::cerr << "Usage: "<< program_name <<  " [input_file] -o [output_file]";
}

int main(const int argc, const char* argv[]){
    // Parse Arguments
    program_name = argv[0];
    if(argc == 1){ 
        usage();
        return 1;
    };

    std::string in_filename = argv[1];
    std::string out_filename = "a.out";

    int curr_arg = 2;
    while(curr_arg < argc){
        if(std::string(argv[curr_arg++]) == "-o"){
            if(curr_arg >= argc){
                usage(); 
                break;
            }
            out_filename = argv[curr_arg++];
        }
        else {
            usage(); 
            return 1; 
        }
    }
    std::cout << out_filename;

    // Read File
    std::ifstream in(in_filename);
    if(!in){
        std::cerr << "Error opening "  << in_filename << '\n';
        return 1;
    }

    std::ofstream out(out_filename, std::ios::binary);
    if(!out){
        std::cerr << "Error opening " << out_filename << '\n';
    }

    std::vector<std::string> lines;
    std::vector<Instr> program; 
    std::map<std::string, unsigned int> labels;
    

    // Reading lines into vector
    {
        std::string line;
        while (std::getline(in, line)) {
            if (!line.empty() && line.back() == '\r')
                line.pop_back();
            lines.push_back(line);
        }
    }

    std::cout << "Read " << lines.size() << " lines\n";
    size_t pc = 0;

    // Storing labels and stripping comments
    {
        for (size_t i = 0; i < lines.size(); i++) {
            std::string &line = lines[i];
            strip_comment(line);

            if(line.empty())  continue;

            std::stringstream ss(line);
            std::string word;
            ss >> word;

            if(word.empty())  continue;


            if(word.back() == ':'){
                word.pop_back();
                std::cout << "Found label: " << word << " -> instruction " << pc << "\n";
                labels[word] = pc;
                line = "";
                continue;
            } 

            Opcode opcode;
            if(get_opcode(word.c_str(), &opcode)){
                switch (opcode){
                    case OP_LBI:
                    case OP_JMP:
                    case OP_JZ:
                        pc+=2;
                        break;
                    default: 
                        pc++;
                        break;
                }
            }
        }
    }

    std::cout << "Program with comments stripped:\n";
    print_lines(lines);

    // Evaluating lines
    for (size_t i = 0; i < lines.size(); i++) {
        Instr instr{};
        std::string& line = lines[i];

        if(line.empty()) {
            continue;
        }

        std::cout << "Line " << i << ": " << line << "\n";
        std::stringstream ss(line);
        std::string word;
        int word_count = 0;

        // Evaluating words
        while(ss >> word) {
            word_count++;
            if(word_count == 1) {
                //Opcode
                if(get_opcode(word.c_str(), &instr.opcode)){
                    LOG_MSG("Opcode:\t\t%01X <-> %s\n", instr.opcode, word.c_str());
                }
                else{
                    std::cerr << "Invalid opcode " << word << '\n';
                    return 1;
                }
            }
            else{
                const int arg_index = word_count - 2;

                if(instr.opcode == OP_JMP || instr.opcode == OP_JZ){
                    LOG_MSG("Jump to label %s replaced with %d\n", word.c_str(), labels[word]);
                    instr.val[0] = labels[word];
                    break;
                }

                // Registers
                else if(word[0] == 'R' ){
                    LOG_MSG("Argument %d:\t%s (Register)\n", arg_index, word.c_str());
                    instr.val[arg_index] = std::stoi(word.substr(1));
                    instr.type[arg_index] = REGISTER;
                }

                // Immediate
                else {
                    LOG_MSG("Argument %d:\t%s (Immediate)\n", arg_index, word.c_str());
                    instr.val[arg_index] = std::stoi(word);
                    instr.type[arg_index] = IMMEDIATE;
                }
            }
        }
        if(word_count == 0) continue;
        program.push_back(instr);
    }

    //Writing instructions
    for(size_t i = 0; i < program.size(); i++) {
        Instr &instr = program[i];
        if(instr.opcode == OP_LBI){
            const uint16_t instr_enc =  ENCODE_INSTR(instr.opcode, (uint8_t) REGISTER , (uint8_t) instr.val[0], (uint8_t) IMMEDIATE, 0);
            out.write((const char *) &instr_enc, sizeof(instr_enc));

            const uint16_t big_imm = instr.val[1];
            out.write((const char *) &big_imm, sizeof(big_imm));
        }

        else if (instr.opcode == OP_JZ || instr.opcode == OP_JMP){
            const uint16_t instr_enc =  ENCODE_INSTR(instr.opcode, (uint8_t) IMMEDIATE, (uint8_t) 0, (uint8_t) IMMEDIATE, 0);
            out.write((const char *) &instr_enc, sizeof(instr_enc));

            const uint16_t big_imm = instr.val[0];
            out.write((const char *) &big_imm, sizeof(big_imm));
        }

        else{
            const uint16_t instr_enc =  ENCODE_INSTR(instr.opcode, (uint8_t) instr.type[0], (uint8_t) instr.val[0], (uint8_t) instr.type[1], (uint8_t) instr.val[1]);
            out.write((const char *) &instr_enc, sizeof(instr_enc));
        }
    }

    in.close();
    out.close();
    return 0;
}
