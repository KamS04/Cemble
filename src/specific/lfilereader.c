#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int exists(char* filename) {
    return access(filename, F_OK);
}

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