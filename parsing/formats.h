#ifndef INSTRUCTION_FORMATS_HEADER
#define INSTRUCTION_FORMATS_HEADER

#include "parselib.h"
#include "instruction.h"

typedef parser*(*insParserCreator)(Instruction*);

extern insParserCreator typeCreatorMap[];

#endif