#ifndef ENCODER_HEADER
#define ENCODER_HEADER

#include<stdint.h>
#include "reader.h"

typedef struct {
    Syntax** parsedAssembly;
    int len;
    ReadOutput* readOutput;
    uint16_t sAddr;
    uint16_t cAddr;
    Map registersMap;
} AssemblyContext;

uint8_t* encodeAsm(AssemblyContext* asmCon);

#endif