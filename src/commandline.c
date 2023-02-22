#include<stdio.h>
#include<stdlib.h>
#include<string.h>
// #include<time.h>
#include<sys/timeb.h>
#include "assemblelib.h"
#include "parselib.h"
#include "assembler.h"
#include "writer.h"
#include "timing.h"
#include "setup.h"
#include "filereader.h"

typedef struct {
    bool debug;
    bool printAST;
    bool decimalAddrs;
    char* file;
    char* output;
    char* codeOffset;
    char* astFile;
} Options;

typedef enum {
    file,
    output,
    codeOffset,
    astFile
} LookingAt;

typedef enum {
    EXPECTING_FLAG_OR_NAME,
    EXPECTING_ARGUMENT
} OptParseState;

void write_help() {
    puts("printing help");
}

void parse_arguments(int argc, char* argv[], Options* out) {
    OptParseState ops = EXPECTING_FLAG_OR_NAME;
    LookingAt lA;

    for (int i = 0; i < argc; i++) {
        char* o = argv[i];
        if (ops == EXPECTING_FLAG_OR_NAME) {
            // Help
            if (strcmp(o, "-h") == 0 || strcmp(o, "--help") == 0) {
                write_help();
                exit(0);
            }
            // Flags
            else if (strcmp(o, "-d") == 0) {
                out->debug = true;
            } else if (strcmp(o, "-p") == 0) {
                out->printAST = true;
            } else if (strcmp(o, "--dec") == 0) {
                out->decimalAddrs = true;
            }
            // Arguments
            else if (strcmp(o, "-f") == 0) {
                lA = file;
                ops = EXPECTING_ARGUMENT;
            } else if (strcmp(o, "-o") == 0) {
                lA = output;
                ops = EXPECTING_ARGUMENT;
            } else if (strcmp(o, "-c") == 0) {
                lA = codeOffset;
                ops = EXPECTING_ARGUMENT;
            } else if (strcmp(o, "-a") == 0) {
                lA = astFile;
                ops = EXPECTING_ARGUMENT;
            }
        } else {
            switch(lA) {
                case file:
                    out->file = o;
                    break;
                case output:
                    out->output = o;
                    break;
                case codeOffset:
                    out->codeOffset = o;
                    break;
                case astFile:
                    out->astFile = o;
            }
            ops = EXPECTING_FLAG_OR_NAME;
        }
    }



    if (ops == EXPECTING_ARGUMENT) {
        puts("Expected argument but found end of commandline arguments instead");
        exit(3);
    }
}

void writeb(uint8_t t) {
    putchar((t >> 7    ) + '0');
    putchar((t >> 6 & 1) + '0');
    putchar((t >> 5 & 1) + '0');
    putchar((t >> 4 & 1) + '0');
    putchar((t >> 3 & 1) + '0');
    putchar((t >> 2 & 1) + '0');
    putchar((t >> 1 & 1) + '0');
    putchar((t      & 1) + '0');
    putchar('\n');
}

int main(int argc, char* argv[]) {
    init_timer();

    Options opt;
    opt.file = NULL;
    opt.output = NULL;
    opt.codeOffset = NULL;
    opt.astFile = NULL;
    opt.debug = false;
    opt.printAST = false;
    opt.decimalAddrs = false;    
    
    parse_arguments(argc, argv, &opt);

    FILE* astOut;
    if (opt.astFile) {
        astOut = fopen(opt.astFile, "w");
    } else {
        astOut = stdout;
    }

    uint16_t cOffset = 0;
    if (opt.codeOffset) {
        sscanf(opt.codeOffset, "%hu", &cOffset);
    }

    DebugFlags deb = config_debug(opt.debug, opt.printAST, opt.decimalAddrs);

    if (opt.file == NULL) {
        puts("No input file specified");
        exit(3);
    } else if (!exists(opt.file)) {
        puts("Input file does not exist");
        exit(3);
    }

    char* assembly = read_assembly_file(opt.file);

    setup();
    AssemblyResult* aRes = cassemble(assembly, cOffset, deb, astOut);

    if (opt.astFile) {
        fclose(astOut);
    }

    putchar('\n');

    if (opt.output) {
        FILE* outFile = fopen(opt.output, "wb");
        fwrite(aRes->mCode, aRes->lenMC, 1, outFile);
        fclose(outFile);
    } else {
        for (int i = 0; i < aRes->lenMC; i++) {
            if (i != 0 & i % 8 == 0) {
                putchar('\n');
            }
            uint8_t byte = aRes->mCode[i];
            putchar( hexC[byte >> 4] );
            putchar( hexC[byte & 0x0f] );
            putchar(' ');
        }
    }
    putchar('\n');

    long long elapsed = get_elapsed_time();
    printf("Assembly completed in %lld milliseconds\n", elapsed);
}
