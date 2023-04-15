#ifndef READER_HEADER
#define READER_HEADER

#include "assemblelib.h"
#include<stdint.h>
#include "hashmap.h"

typedef struct CountOutput {
    int structures;
    int symbols;
    int lines;
    int exports;
    int exportStructures;
} CountOutput;

typedef struct ReadOutput {
    // char* -> Int
    // internal // intialized
    HashMap* symbols;
    // char* -> StructureComp*
    // internal
    HashMap* structures;
    // char* -> Syntax*
    // internal + external // initialized
    HashMap* tbinit_symbols;
    // Hashset of of global export names
    HashMap* exports;
    // Hashset of global exported structures
    HashMap* exportStructs;
    // \n -> start_of_line
    int* nextLineIndexes;
    // where to break in the machineCode
    int num_next_lines;
    uint16_t cAddr;
    CountOutput cData;
} ReadOutput;

typedef struct StructureComp {
    char* name;
    // char* -> StructMember
    HashMap *members;
    uint16_t totalSize;
} StructureComp;

typedef struct StructMember {
    char* name;
    uint16_t offset;
    uint16_t size;
} StructMember;

// ReadOutput* readRun(Syntax** pa78rsedAssembly, int len, char* asmC, uint16_t cOffset);
// CountOutput* countRun(Syntax** parsedAssembly, int len);

// TODO fix this
#include "context.h"

void mf_countRun(Syntax **parsedAssembly, int len, CountOutput *out);
int mf_readRun(Syntax **parsedAssembly, int len, ReadOutput *out, char **error);
int complete_initialize(SParseRead *parseReads, int len_f, HashMap *exportSymbols, HashMap *exportStructures, char **error, char **efile);

#endif