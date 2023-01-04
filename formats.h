#ifndef FORMATS_HEADER
#define FORMATS_HEADER

#include "parselib.h"
#include "assemblelib.h"

// required by assemblelib
typedef parser*(*insParserCreator)(Instruction*);
extern insParserCreator typeCreatorMap[];

// actual implementation

// Basic Creators/Mappers
parser* createSingle(Instruction* ins, parser* arg);
parser* createDoubles(Instruction* ins, parser* arg1, parser* arg2);
parser* createTriple(Instruction* ins, parser* arg1, parser* arg2, parser* arg3);

// Type Creators
parser* fNoArgs(Instruction* ins);
parser* fSingleLit(Instruction* ins);
parser* fSingleReg(Instruction* ins);
parser* fSingleMem(Instruction* ins);
parser* fLitReg(Instruction* ins);
parser* fRegLit(Instruction* ins);
parser* fRegReg(Instruction* ins);
parser* fRegMem(Instruction* ins);
parser* fMemReg(Instruction* ins);
parser* fLitMem(Instruction* ins);
parser* fRegPtrReg(Instruction* ins);
parser* fRegRegPtr(Instruction* ins);
parser* fRegPtr(Instruction* ins);
parser* fLitOffReg(Instruction* ins);

InstructionItem* create_insitem(Instruction* ins, Syntax** args, int args_s);

#endif