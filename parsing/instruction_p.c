#include<stdlib.h>
#include "instruction.h"
#include "formats.h"

parser** instructionParsers;
parser* _instructionParser = NULL;

size_t instruction_size(InstructionType t) {
    return instructionSizes[t];
}

parser* createInstructionParser(Instruction* ins) {
    return typeCreatorMap[ins->type](ins);
}

parser** _createInstructionParser() {
    instructionParsers = malloc( NUM_INSTRUCTIONS * sizeof(parser*) );
    for (int i = 0; i < NUM_INSTRUCTIONS; i++) {
        instructionParsers[i] = createInstructionParser(&instructions[i]);
    }
    return instructionParsers;
}

parser* instructionParser() {
    if (_instructionParser == NULL) {
        _createInstructionParser();
        _instructionParser = choice(instructionParsers, NUM_INSTRUCTIONS);
    }
    return _instructionParser;
}
