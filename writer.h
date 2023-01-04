#ifndef ASSEMBLY_WRITER_HEADER
#define ASSEMBLY_WRITER_HEADER

#include "parselib.h"
#include<stdint.h>
#include "encoder.h"

extern char hexC[];

int find_n_after(char* ascM, int st);

void pretty_print_asm_error(char* asmC, state* pA);
void pretty_print_machine_code(uint8_t* mCode, uint8_t cOffset, int mLen, char* asmC, AssemblyContext* asmCon, int decAddrs);

#endif