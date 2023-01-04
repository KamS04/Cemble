#ifndef ENCODE_HEADER
#define ENCODE_HEADER

#include<stdint.h>
#include "assemblelib.h"
#include "encoder.h"

uint16_t evaluate_node(Syntax* syn, Map symbols, Map structures);

void encodeInstruction(uint8_t* mCode, AssemblyContext* asmCon, InstructionItem* iex);

void encodeData(uint8_t* mCode, AssemblyContext* asmCon, DataElement* dex);
void encodeLitOrMem(uint8_t* mCode, AssemblyContext* asmCon, Syntax* lit);
void encodeLit8OrMem(uint8_t* mCode, AssemblyContext* asmCon, Syntax* lit);
void encodeReg(uint8_t* mCode, AssemblyContext* asmCon, Syntax* reg);

#endif