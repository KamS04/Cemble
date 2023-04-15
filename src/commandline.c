#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>

#include "assemblelib.h"
#include "parselib.h"
#include "assembler.h"
#include "writer.h"
#include "timing.h"
#include "setup.h"
#include "filereader.h"

typedef struct Options {
    bool debug;
    bool printAST;
    bool decimalAddrs;
    char* output;
    uint16_t codeOffset;
    char* astFile;
    char** files;
    int len_f;
} Options;

typedef enum LookingAt {
    FILE_E,
    OUTPUT,
    CODEOFFSET,
    ASTFILE
} LookingAt;

typedef enum OptParseState {
    EXPECTING_FLAG,
    EXPECTING_FLAG_OR_ARGUMENT,
    EXPECTING_ARGUMENT
} OptParseState;

void write_help() {
    puts("Usage: fcasm [options]");
    puts("\toptions:");
    puts("\t\t-h, --help          Prints usage");
    puts("\t\t-d                  Enables Debug mode");
    puts("\t\t-p                  Prints AST after parse step");
    puts("\t\t--dec               In Debug mode, print addresses as decimal values instead of hex");
    puts("\t\t-f file1 [files...] Files to assemble");
    puts("\t\t-o                  Output file");
    puts("\t\t-c offset           Start address for code defaults to 0");
    puts("\t\t-a file             Write AST to file");
}

int parse_arguments(int argc, char* argv[], Options* out) {
    #define opteq(x) strcmp(o, x) == 0
    OptParseState ops = EXPECTING_FLAG;
    LookingAt lA;

    int st;

    for (int i = 1; i < argc; i++) {
        char* o = argv[i];

        switch (ops) {
            case EXPECTING_FLAG: {
                if (opteq("-h") || opteq("--help")) {
                    write_help();
                    return -1;
                } else if (opteq("-d")) { // Flags
                    out->debug = true;
                } else if (opteq("-p")) {
                    out->printAST = true;
                } else if (opteq("--dec")) {
                    out->decimalAddrs = true;
                } else if (opteq("-f")) {// Arguments
                    lA = FILE_E;
                    ops = EXPECTING_ARGUMENT;
                } else if (opteq("-o")) {
                    lA = OUTPUT;
                    ops = EXPECTING_ARGUMENT;
                } else if (opteq("-c")) {
                    lA = CODEOFFSET;
                    ops = EXPECTING_ARGUMENT;
                } else if (opteq("-a")) {
                    lA = ASTFILE;
                    ops = EXPECTING_ARGUMENT;
                } else {
                    printf("Unknown options %s\n", o);
                }
                break;
            }

            case EXPECTING_ARGUMENT: {
                if (lA == FILE_E) {
                    st = i;
                    out->files = &argv[st];
                    ops = EXPECTING_FLAG_OR_ARGUMENT;
                } else if (lA == OUTPUT) {
                    out->output = o;
                    ops = EXPECTING_FLAG;
                } else if (lA == CODEOFFSET) {
                    sscanf(o, "%hu", &out->codeOffset);
                    ops = EXPECTING_FLAG;
                } else if (lA == ASTFILE) {
                    out->astFile = o;
                    ops = EXPECTING_FLAG;
                }
                break;
            }

            case EXPECTING_FLAG_OR_ARGUMENT: {
                if (lA == FILE_E) {
                    if (o[0] == '-' || i + 1 == argc) {
                        // ended file list, now at flags again
                        if (i + 1 == argc) {
                            out->len_f = i - st + 1;
                        } else {
                            out->len_f = i - st - 1;
                            i--; // Will process this flag again
                        }
                        ops = EXPECTING_FLAG;
                    }
                } else {
                    printf("Argument parser entered odd state at %s\n", o);
                }
                break;
            }
        }
    }

    if (ops == EXPECTING_ARGUMENT) {
        puts("Expected argument but found end of commandline arguments instead");
        return -1;
    }

    return 1;
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
    opt.files = NULL;
    opt.output = NULL;
    opt.codeOffset = 0;
    opt.astFile = NULL;
    opt.debug = false;
    opt.printAST = false;
    opt.decimalAddrs = false;    
    
    int psuc = parse_arguments(argc, argv, &opt);
    if (psuc == -1) {
        return 3;
    }

    FILE* astOut;
    if (opt.astFile) {
        astOut = fopen(opt.astFile, "w");
    } else {
        astOut = stdout;
    }

    DebugFlags deb = config_debug(opt.debug, opt.printAST, opt.decimalAddrs);

    if (opt.files == NULL) {
        puts("No input file specified");
        return 3;
    }

    bool fne = false;
    for (int i = 0; i < opt.len_f; i++) {
        if (!exists(opt.files[i])) {
            printf("File: %s, does not exist\n", opt.files[i]);
            fne = true;
        }
    }

    if (fne) {
        // TODO do stuff
        exit(3);
    }

    // Assembler will start reading files within the file specific threads
    // char* assembly = read_assembly_file(opt.files[0]);

    // setup(); // Assembler will take care of it now
    AssemblingResult aRes;
    cassemble_mt(opt.files, opt.len_f, opt.codeOffset, deb, astOut, &aRes);
    // AssemblyResult* aRes = cassemble(assembly, opt.codeOffset, deb, astOut);

    if (opt.astFile != NULL) {
        fclose(astOut);
    }

    putchar('\n');

    if (opt.output != NULL) {
        FILE* outFile = fopen(opt.output, "wb");
        fwrite(aRes.mCode, aRes.machineLength, 1, outFile);
        fclose(outFile);
    } else {
        for (int i = 0; i < aRes.machineLength; i++) {
            if ( (i != 0) & (i % 8 == 0) ) {
                putchar('\n');
            }
            uint8_t byte = aRes.mCode[i];
            putchar( hexC[byte >> 4] );
            putchar( hexC[byte & 0x0f] );
            putchar(' ');
        }
    }
    putchar('\n');

    long long elapsed = get_elapsed_time();
    printf("Assembly completed in %lld milliseconds\n", elapsed);

    return 0;
}
