#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>

int exists(char* filename) {
    struct stat buffer;
    int exist = stat(filename, &buffer);
    return (exist == 0) && S_ISREG(buffer.st_mode);
}



// int exists(char* filename) {
//     return access(filename, F_OK) == 0;
// }

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