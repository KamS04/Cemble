#ifndef ENCODER_HEADER
#define ENCODER_HEADER

#include<stdint.h>
#include "reader.h"
#include "context.h"

int encoderOfAsm(AssemblingContext asmCon, uint8_t *mCode, HashMap *exportSymbols, HashMap *exportStructures);

#endif