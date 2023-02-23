#include<stdint.h>
#include<stdbool.h>
#include<stdio.h>
#include<string.h>
#include "parselib.h"
#include "assemblelib.h"
#include "assembler.h"
#include "reader.h"
#include "encoder.h"
#include "writer.h"
#include "sort.h"

#define odec(x) x & 0x30
#define oast(x) x & 0x0c
#define odebug(x) x & 0x03

char* registerNames[] = {
    "ip", "acu",
    "r1", "r2", "r3", "r4",
    "r5", "r6", "r7", "r8",
    "sp", "fp", "mb", "im"
};
int registerIndexes[] = {
    0,  1,  2,  3,  4,  5,  6,
    7,  8,  9, 10, 11, 12
};

DebugFlags config_debug(bool debug, bool pAst, bool dAddrs) {
    return dAddrs << 4 | pAst << 2 | debug;
}

AssemblyResult* assemble(char* asmC, uint16_t cOffset, DebugFlags debug) {
    return cassemble(asmC, cOffset, debug, stdout);
}

AssemblyResult* cassemble(char* asmC, uint16_t cOffset, DebugFlags debug, FILE* aout) {
    state* sn = run(create_assembly_parser(), asmC);
    if (sn->is_error || asmC[sn->index] != '\0') {
        puts("Assembly had a syntax error");
        printf("Failed at %d\n", sn->index);
        pretty_print_asm_error(asmC, sn);
        exit(3);
    }

    ResArrD* orad = sn->result->data.ptr;
    int len = orad->a_len;
    Syntax** parsedAss = (Syntax**)orad->arr;
    
    if (oast(debug)) {
        char* s;
        for (int i = 0; i < len; i++) {
            s = syntax_to_string(parsedAss[i], true);
            fputs(s, aout);
            // fprintf(aout, "%s\n", s);
            putc('\n', aout);
            free(s);
        }
    }

    ReadOutput* ro = readRun(parsedAss, len, asmC, cOffset);

    if (odebug(debug)) {
        puts("SymbolicNames: ");
        for (int i = 0; i < ro->symbols->length; i++) {
            printf("%s, ", ro->symbols->arr[i].key );
        }
        putchar('\n');

        puts("Structures: ");
        for (int i = 0; i < ro->structures->length; i++) {
            printf("%s, ", ro->structures->arr[i].key);
        }
        putchar('\n');
        putchar('\n');
    }

    AssemblyContext* asmCon = malloc(sizeof(AssemblyContext));
    asmCon->parsedAssembly = parsedAss;
    asmCon->len = len;
    asmCon->readOutput = ro;
    asmCon->sAddr = cOffset;
    asmCon->registersMap = simplemap_int(registerNames, registerIndexes, 14);

    uint8_t* mCode = encodeAsm(asmCon);
    int mLen = ro->cAddr - cOffset;

    if (odebug(debug)) {
        // write out machine code array
        printf("MachineCodeArray(");
        for (int i = 0; i < mLen; i++) {
            printf("%02d, ", mCode[i]);
        }
        printf(")\n\n");

        pretty_print_machine_code(mCode, cOffset, mLen, asmC, asmCon, odec(debug));
    }

    AssemblyResult* ar = malloc(sizeof(AssemblyResult));
    ar->readoutput = ro;
    ar->lenMC = mLen;
    ar->mCode = mCode;
    ar->ast = parsedAss;
    ar->lenAST = len;

    return ar;
}
