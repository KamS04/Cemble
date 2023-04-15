#include<stdint.h>
#include<stdbool.h>
#include<stdio.h>
#include<string.h>
#include<pthread.h>

#include "parselib.h"
#include "assemblelib.h"
#include "debugging.h"
#include "assembler.h"
#include "reader.h"
#include "encoder.h"
#include "writer.h"
#include "sort.h"
#include "setup.h"
#include "filereader.h"
#include "hashmap.h"
#include "context.h"

#define odec(x) x & 0x30
#define oast(x) x & 0x0c
#define odebug(x) x & 0x03

#define SIZEOFARR(x, t) sizeof(x) / sizeof(t)

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

#if 0
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
            kfree(s);
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
#endif

void* parseFile(SParseRead* out) {
    out->asmF = read_assembly_file(out->filename);
    out->parse = run(create_assembly_parser(), out->asmF);
    ResArrD* orad = out->parse->result->data.ptr;
    out->lenAst = orad->a_len;
    out->ast = malloc(orad->a_len * sizeof(Syntax*));
    for (int i = 0; i < orad->a_len; i++) {
        out->ast[i] = orad->arr[i].ptr;
    }

    // We don't need the array anymore
    kfree(orad->arr);
    // We don't need the orad anymore;
    kfree(orad);
    return NULL;
}

void* readParsed(SParseRead* out) {
    out->state = mf_readRun(out->ast, out->lenAst, &out->readOutput, &out->error);
    return NULL;
}

int cassemble_mt(char** filenames, int len_f, uint16_t cOffset, DebugFlags debug, FILE* astOut, AssemblingResult *asmRes) {
    setup();

    // Hopefully everything is now ready
    create_assembly_parser();

    #if defined RELEASE || defined DEBUG
    pthread_t thread_ids[len_f];
    #else
    pthread_t *thread_ids = malloc( len_f * sizeof(pthread_t) );
    #endif

    SParseRead *parseReads = malloc( len_f * sizeof(SParseRead) ); // would make this an array
    // but have actual problems when outputting

    // #region First Parse all the ASTs -- Multithreaded
    if (len_f == 1) {
        parseReads[0].idx = 0;
        parseReads[0].filename = filenames[0];
        parseFile(&parseReads[0]);
        readParsed(&parseReads[0]);
    } else {
        for (int i = 0; i < len_f; i++) {
            parseReads[i].idx = i;
            parseReads[i].filename = filenames[i];
            pthread_create(&thread_ids[i], NULL, parseFile, &parseReads[i]);
        }

        for (int i = 0; i < len_f; i++) {
            pthread_join(thread_ids[i], NULL);
        }
    }

    bool err = false;
    for (int i = 0; i < len_f; i++) {
        state* sn = parseReads[i].parse;
        if (sn->is_error || parseReads[i].asmF[sn->index] != '\0') {
            if (!err) {
                puts("Assembly had a syntax error\n");
            }

            printf("%s - Failed at %d\n", parseReads[i].filename, sn->index);
            pretty_print_asm_error(parseReads[i].asmF, sn);
            puts("\n");
            err = true;
        }
    }

    if (err) {
        // TODO In the future deallocate everything
        // Yea we ain't getting there
        exit(3);
    }

    if (oast(debug)) {
        for (int i = 0; i < len_f; i++) {
            printf("Syntax : %s\n", parseReads[i].filename);
            pretty_print_syntax(parseReads[i].ast, parseReads[i].lenAst, astOut);
            putchar('\n');
        }
    }
    // #endregion

    // #region Second Read through all the ASTs -- Multithreaded
    if (len_f != 1) {
        for (int i = 0; i < len_f; i++) {
            pthread_create(&thread_ids[i], NULL, readParsed, &parseReads[i]);
        }

        for (int i = 0; i < len_f; i++) {
            pthread_join(thread_ids[i], NULL);
        }
    }
    // #endregion

    // #region Third initialized the uninitialized Variables -- Singlethreaded
    int totalexportstructures = 0;
    int totalexportsymbols = 0;

    err = false;
    uint8_t currentOffset = cOffset;

    for (int i = 0; i < len_f; i++) {
        if (parseReads[i].state == -1) {
            err = true;
            printf("Reading failed for %s, Error:\n", parseReads[i].filename);
            puts(parseReads[i].error);
        }
        totalexportstructures += parseReads[i].readOutput.cData.exportStructures;
        totalexportsymbols += parseReads[i].readOutput.cData.exports;

        // update segment offsets
        parseReads[i].segmentOffset = currentOffset;
        currentOffset += parseReads[i].readOutput.cAddr; // sum up the cOffs
    }

    if (err) {
        // TODO in the future deallocate stuff
        // see other comment
        exit(3);
    }

    HashMap *exportSymbols = hashmap_int(NULL, NULL, 0, totalexportsymbols);
    HashMap *exportStructures = hashmap_ptr(NULL, NULL, 0, totalexportstructures);

    // Initialize stuff
    char *error;
    char *efile;
    int err_state = complete_initialize(parseReads, len_f, exportSymbols, exportStructures, &error, &efile);
    
    if (err_state != 1) {
        printf("Error Ocurred in: %s\n", efile);
        printf("Error:\n%s\n", error);
        exit(3);
    }
    
    // #endregion

    // #region Fourth Encode -- Singlethreaded
    uint8_t byteLength = currentOffset - cOffset;
    uint8_t *mCode = malloc(byteLength * sizeof(uint8_t));

    AssemblingContext asmCon;

    asmCon.codeSize = byteLength;
    asmCon.cOffset = cOffset;
    asmCon.len_f = len_f;
    asmCon.sparseReads = parseReads;
    asmCon.registersMap = hashmap_int(registerNames, registerIndexes, SIZEOFARR(registerNames, char*), SIZEOFARR(registerNames, char*));

    encoderOfAsm(asmCon, mCode, exportSymbols, exportStructures);

    // #endregion

    // #region Finish

    memcpy(&asmRes->asmCon, &asmCon, sizeof(AssemblingContext));
    asmRes->mCode = mCode; // whoops
    asmRes->sparseReads = parseReads;
    asmRes->machineLength = byteLength;
    asmRes->cOffset = cOffset;
    asmRes->len_f = len_f;

    if (odebug(debug)) {
        puts("MachineCodeArray()");
        for (int i = 0; i < byteLength; i++) {
            printf("%02d, ", mCode[i]);
        }
        puts(")\n\n");

        mf_pretty_print_machine_code(asmRes, odec(debug));
    }


    // #endregion

    return 1;
}