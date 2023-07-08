#include "instruction.h"
#include<stdio.h>
// #include "formats.h"

Instruction instructions[] = {
    { "LOG_REG",          0x20, singleReg,  "log" },
    { "LOG_REG_PTR",      0x21, regPtr,     "log" },
    { "LOG_MEM",          0x40, singleMem,  "log" },

    { "MOV8_LIT_MEM",     0xD9, lit8Mem,    "mov8" },
    { "MOV8_MEM_REG",     0x9B, memReg,     "mov8" },
    { "MOVL_REG_MEM",     0x79, regMem,     "movl" },
    { "MOVH_REG_MEM",     0x7D, regMem,     "movh" },
    { "MOV8_REG_PTR_REG", 0xBF, regPtrReg,  "mov8" },
    { "MOV8_REG_REG_PTR", 0xBB, regRegPtr,  "mov8" },

    { "MOV_LIT_REG",      0x98, litReg,     "mov" },
    { "MOV_REG_REG",      0xB8, regReg,     "mov" },
    { "MOV_REG_REG_PTR",  0xBA, regRegPtr,  "mov" },
    { "MOV_REG_MEM",      0x78, regMem,     "mov" },
    { "MOV_MEM_REG",      0x9A, memReg,     "mov" },
    { "MOV_LIT_MEM",      0xD8, litMem,     "mov" },
    { "MOV_REG_PTR_REG",  0xBE, regPtrReg,  "mov" },
    { "MOV_LIT_OFF_REG",  0xF8, litOffReg,  "mov" },

    { "INT_LIT",          0x45, singleLit,  "int" },
    { "INT_REG",          0x25, singleReg,  "int" },
    { "RET_INT",          0x05, noArgs,     "rti" },

    { "ADD_REG_REG",      0xB0, regReg,     "add" },
    { "ADD_LIT_REG",      0x90, litReg,     "add" },

    { "SUB_LIT_REG",      0x91, litReg,     "sub" },
    { "SUB_REG_LIT",      0x71, regLit,     "sub" },
    { "SUB_REG_REG",      0xB1, regReg,     "sub" },

    { "INC_REG",          0x30, singleReg,  "inc" },
    { "DEC_REG",          0x31, singleReg,  "dec" },
    { "MUL_LIT_REG",      0x96, litReg,     "mul" },
    { "MUL_REG_REG",      0xB6, regReg,     "mul" },

    { "LSF_REG_LIT",      0x74, regLit8,    "lsf" },
    { "LSF_REG_REG",      0xB4, regReg,     "lsf" },
    { "RSF_REG_LIT",      0xB5, regLit8,    "rsf" },
    { "RSF_REG_REG",      0x75, regReg,     "rsf" },

    { "AND_REG_LIT",      0x77, regLit,     "and" },
    { "AND_REG_REG",      0xB7, regReg,     "and" },
    { "OR_REG_LIT",       0x72, regLit,     "or" },
    { "OR_REG_REG",       0xB2, regReg,     "or" },
    { "XOR_REG_LIT",      0x73, regLit,     "xor" },
    { "XOR_REG_REG",      0xB3, regReg,     "xor" },
    { "NOT",              0x37, singleReg,  "not" },

    { "JMP_REG",          0x28, singleReg,  "jmp" },
    { "JMP_LIT",          0x41, singleMem,  "jmp" },
    { "JMP_NOT_EQ",       0xCD, litMem,     "jne" },
    { "JNE_REG",          0x6D, regMem,     "jne" },
    { "JEQ_REG",          0x6A, regMem,     "jeq" },
    { "JEQ_LIT",          0xCA, litMem,     "jeq" },
    { "JLT_REG",          0x6C, regMem,     "jlt" },
    { "JLT_LIT",          0xCC, litMem,     "jlt" },
    { "JGT_REG",          0x69, regMem,     "jgt" },
    { "JGT_LIT",          0xC9, litMem,     "jgt" },
    { "JLE_REG",          0x6E, regMem,     "jle" },
    { "JLE_LIT",          0xCE, litMem,     "jle" },
    { "JGE_REG",          0x6B, regMem,     "jge" },
    { "JGE_LIT",          0xCB, litMem,     "jge" },

    { "PSH_LIT",          0x42, singleLit,  "psh" },
    { "PSH_REG",          0x22, singleReg,  "psh" },
    { "POP",              0x23, singleReg,  "pop" },

    { "CAL_LIT",          0x44, singleLit,  "cal" },
    { "CAL_REG",          0x24, singleReg,  "cal" },
    { "RET",              0x04, noArgs,     "ret" },
    { "HLT",              0x06, noArgs,     "hlt" },
    { "BRK",              0x03, noArgs,     "brk" },
    { "NOP",              0x00, noArgs,     "nop" },

    { "MEM_MODE_SET",     0x47, singleLit8, "mms" },
    { "SEND_SIG",         0x27, singleReg,  "sig" },
    { "REAL_REG_PTR",     0xA7, regReg,     "rav" },
    { "REAL_MEM",         0x87, memReg,     "rav" }
};
int NUM_INSTRUCTIONS = 67;

size_t instructionSizes[] = {
    4, 4, 3, 3, 4, 4, 5, 4, 3, 5, 1, 2, 3, 3, 3, 2, 2
};

char* instTypeStrs[] = {
    "litReg",
    "regLit",
    "regLit8",
    "regReg",
    "regMem",
    "memReg",
    "litMem",
    "lit8Mem",
    "regPtrReg",
    "litOffReg",
    "noArgs",
    "singleReg",
    "singleLit",
    "regRegPtr",
    "singleMem",
    "regPtr",
    "singleLit8"
};
