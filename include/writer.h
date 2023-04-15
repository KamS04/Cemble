#ifndef ASSEMBLY_WRITER_HEADER
#define ASSEMBLY_WRITER_HEADER

#include<stdint.h>
#include<stdio.h>

#include "parselib.h"
#include "assemblelib.h"
#include "encoder.h"

extern char hexC[];

int find_n_after(char* ascM, int st);

void pretty_print_asm_error(char* asmC, state* pA);
void pretty_print_syntax(Syntax** parsedAss, int len, FILE* aout);

void mf_pretty_print_machine_code(AssemblingResult *asmRes, int decAddrs);

#endif