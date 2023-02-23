#ifndef INSTRUCTIONS_HEADER
#define INSTRUCTIONS_HEADER

#include<stdint.h>

typedef enum InstructionType {
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

typedef struct Instruction {
    char tag[17];
    uint8_t opcode;
    InstructionType type;
    char mnemonic[5];
} Instruction;

extern Instruction instructions[];
extern int NUM_INSTRUCTIONS;

extern size_t instructionSizes[];

#endif