#ifndef CONTEXT_HEADER
#define CONTEXT_HEADER

#include<stdint.h>

#include "reader.h"
#include "assemblelib.h"
#include "hashmap.h"

typedef struct SParseRead {
    char* filename;
    int idx;
    char* asmF;
    Syntax** ast;
    state* parse;
    int lenAst;
    int lenMC;
    ReadOutput readOutput;
    char* error;
    int state;
    uint8_t segmentOffset;
} SParseRead;

typedef struct EvalContext {
    HashMap *exportSymbols;
    HashMap *exportStructures;
    HashMap *conSymbols;
    HashMap *conStructures;
    uint16_t startAddr;
    uint16_t currAddr;
    uint16_t codeSize;
    HashMap *registerNames;
} EvalContext;

typedef struct AssemblingContext {
    SParseRead *sparseReads;
    int len_f;
    uint8_t cOffset;
    uint8_t codeSize;
    HashMap* registersMap;
} AssemblingContext;

typedef struct AssemblingResult {
    uint8_t *mCode;
    SParseRead *sparseReads;
    int len_f;
    int machineLength;
    AssemblingContext asmCon;
    uint16_t cOffset;
} AssemblingResult;

#endif