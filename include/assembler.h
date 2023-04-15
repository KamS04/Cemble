#ifndef ASSEMBLE_HEADER
#define ASSEMBLE_HEADER

#include<stdint.h>
#include<stdbool.h>
#include "reader.h"

typedef struct AssemblyResult {
    ReadOutput* readoutput;
    int lenMC;
    uint8_t* mCode;
    Syntax** ast;
    int lenAST;
} AssemblyResult;

typedef struct MTAssemblyResult {
    char** filenames;
    int len_f;
    ReadOutput** readoutput;
    int lenMC;
    uint8_t* mCode;
    Syntax*** ast;
    int* lenAST;
} MTAssemblyResult;

typedef uint8_t DebugFlags;

DebugFlags config_debug(bool debug, bool pAst, bool dAddrs);

// pass 0 for debug if no debug required
// AssemblyResult* assemble(char* asmC, uint16_t cOffset, DebugFlags debug);

// AssemblyResult* cassemble(char* asmC, uint16_t cOffset, DebugFlags debug, FILE* aout);

int cassemble_mt(char** filenames, int len_f, uint16_t cOffset, DebugFlags debug, FILE* astOut, AssemblingResult *asmRes);

#endif