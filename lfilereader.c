#include<stdio.h>
#include<stdlib.h>

char* read_assembly_file(char* filename) {
    FILE* inputFile = fopen(filename, "rb");
    fseek(inputFile, 0, SEEK_END);
    long size = ftell(inputFile);
    fseek(inputFile, 0, SEEK_SET);

    char* assembly = malloc( size + 1 );
    fread(assembly, size, 1, inputFile);
    assembly[size] = '\0';
    return assembly;
}