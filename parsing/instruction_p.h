#ifndef INSTRUCTION_PARSER_HEADER
#define INSTRUCTION_PARSER_HEADER

#include<stdlib.h>
#include "parselib.h"
#include "instruction.h"

size_t instruction_size(InstructionType t);

extern parser** instructionParsers;

parser* createInstructionParser(Instruction* ins);

parser* instructionParser();

#endif