#ifndef READER_HEADER
#define READER_HEADER

#include "map.h"
#include "assemblelib.h"
#include<stdint.h>

typedef struct {
    // char* -> Int
    Map symbols;
    // char* -> StructureComp*
    Map structures;
    // \n -> start_of_line
    int* nextLineIndexes;
    // where to break in the machineCode
    int num_next_lines;
    uint16_t cAddr;
} ReadOutput;

typedef struct {
    int structures;
    int symbols;
    int lines;
} CountOutput;

typedef struct {
    char* name;
    // char* -> StructMember
    Map members;
    uint16_t totalSize;
} StructureComp;

typedef struct {
    char* name;
    uint16_t offset;
    uint16_t size;
} StructMember;

ReadOutput* readRun(Syntax** parsedAssembly, int len, char* asmC, uint16_t cOffset);
CountOutput* countRun(Syntax** parsedAssembly, int len);

#endif