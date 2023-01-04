#include "instruction.h"
#include<stdio.h>
#include "formats.h"

Instruction instructions[] = {
    { "LOG_REG", 0x50, singleReg, "log" },
    { "LOG_REG_PTR", 0x51, regPtr, "log" },
    { "LOG_MEM", 0x52, singleMem, "log" },

    { "MOV8_LIT_MEM", 0x71, lit8Mem, "mov8" },
    { "MOV8_MEM_REG", 0x72, memReg, "mov8" },
    { "MOVL_REG_MEM", 0x73, regMem, "movl" },
    { "MOVH_REG_MEM", 0x74, regMem, "movh" },
    { "MOV8_REG_PTR_REG", 0x76, regPtrReg, "mov8" },
    { "MOV8_REG_REG_PTR", 0x76, regRegPtr, "mov8" },

    { "MOV_LIT_REG", 0x10, litReg, "mov" },
    { "MOV_REG_REG", 0x11, regReg, "mov" },
    { "MOV_REG_REG_PTR", 0x70, regRegPtr, "mov" },
    { "MOV_REG_MEM", 0x12, regMem, "mov" },
    { "MOV_MEM_REG", 0x13, memReg, "mov" },
    { "MOV_LIT_MEM", 0x1b, litMem, "mov" },
    { "MOV_REG_PTR_REG", 0x1c, regPtrReg, "mov" },
    { "MOV_LIT_OFF_REG", 0x1d, litOffReg, "mov" },

    { "INT", 0xfd, singleLit, "int" },
    { "RET_INT", 0xfc, noArgs, "rti" },

    { "ADD_REG_REG", 0x14, regReg, "add" },
    { "ADD_LIT_REG", 0x3f, litReg, "add" },
    { "SUB_LIT_REG", 0x16, litReg, "sub" },
    { "SUB_REG_LIT", 0x1e, regLit, "sub" },
    { "SUB_REG_REG", 0x1f, regReg, "sub" },
    { "INC_REG", 0x35, singleReg, "inc" },
    { "DEC_REG", 0x36, singleReg, "dec" },
    { "MUL_LIT_REG", 0x20, litReg, "mul" },
    { "MUL_REG_REG", 0x21, regReg, "mul" },

    { "LSF_REG_LIT", 0x26, regLit8, "lsf" },
    { "LSF_REG_REG", 0x27, regReg, "lsf" },
    { "RSF_REG_LIT", 0x2a, regLit8, "rsf" },
    { "RSF_REG_REG", 0x2b, regReg, "rsf" },

    { "AND_REG_LIT", 0x2e, regLit, "and" },
    { "AND_REG_REG", 0x2f, regReg, "and" },
    { "OR_REG_LIT", 0x30, regLit, "or" },
    { "OR_REG_REG", 0x31, regReg, "or" },
    { "XOR_REG_LIT", 0x32, regLit, "xor" },
    { "XOR_REG_REG", 0x33, regReg, "xor" },
    { "NOT", 0x34, singleReg, "not" },

    { "JMP_NOT_EQ", 0x15, litMem, "jne" },
    { "JNE_REG", 0x40, regMem, "jne" },
    { "JEQ_REG", 0x3e, regMem, "jeq" },
    { "JEQ_LIT", 0x41, litMem, "jeq" },
    { "JLT_REG", 0x42, regMem, "jlt" },
    { "JLT_LIT", 0x43, litMem, "jlt" },
    { "JGT_REG", 0x44, regMem, "jgt" },
    { "JGT_LIT", 0x45, litMem, "jgt" },
    { "JLE_REG", 0x46, regMem, "jle" },
    { "JLE_LIT", 0x47, litMem, "jle" },
    { "JGE_REG", 0x48, regMem, "jge" },
    { "JGE_LIT", 0x49, litMem, "jge" },

    { "PSH_LIT", 0x17, singleLit, "psh" },
    { "PSH_REG", 0x18, singleReg, "psh" },
    { "POP", 0x1a, singleReg, "pop" },
    
    { "CAL_LIT", 0x5e, singleLit, "cal" },
    { "CAL_REG", 0x5f, singleReg, "cal" },
    { "RET", 0x60, noArgs, "ret" },
    { "HLT", 0xff, noArgs, "hlt" },
    { "BRK", 0xfb, noArgs, "brk" }
};
int NUM_INSTRUCTIONS = 59;

size_t instructionSizes[] = {
    4, 4, 3, 3, 4, 4, 5, 4, 3, 5, 1, 2, 3, 3, 3, 2
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
};
