#ifndef INSTRUCTIONS_HEADER
#define INSTRUCTIONS_HEADER

#include<stdlib.h>
#include<stdint.h>
#include "parselib.h"

// Required by assemblelib

typedef enum {
    litReg,
    regLit,
    regLit8,
    regReg,
    regMem,
    memReg,
    litMem,
    lit8Mem,
    regPtrReg,
    litOffReg,
    noArgs,
    singleReg,
    singleLit,
    regRegPtr,
    singleMem,
    regPtr,
    singleLit8
} InstructionType;

typedef struct {
    char tag[17];
    uint8_t opcode;
    InstructionType type;
    char mnemonic[5];
} Instruction;

extern Instruction instructions[];
extern int NUM_INSTRUCTIONS;

extern size_t instSizes[];

// other stuff
extern char* instTypeStrs[];

#endif