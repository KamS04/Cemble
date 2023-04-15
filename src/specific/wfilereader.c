#include<stdio.h>
#include<stdlib.h>
#include<fileapi.h>
#include<minwindef.h>

// Why can't windows just be unix?????? posix whatever
int exists(char* filename) {
    DWORD dwAttrib = GetFileAttributes(filename);

    return (
        dwAttrib != INVALID_FILE_ATTRIBUTES &&
        !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY)
    );
}

// I hate this entire function
char* read_assembly_file(char* filename) {
    printf("File to be read: %s\n", filename);
    #define BUFFER_SIZE 250

    char* read = malloc( BUFFER_SIZE + 1 );
    long long size = 0;

    read[0] = '\0';

    int readlen = 0;

    FILE* inputFile = fopen(filename, "r");

    while (1) {
        // printf("ca: %s|\n", read);
        readlen = fread(read + size, 1, BUFFER_SIZE, inputFile);
        
        // if we're done the file
        if (readlen < BUFFER_SIZE) {
            if (readlen != 0) {
                // need to remove some of the size of buffer;
                size += readlen;
                read = realloc(read, size + 1);
            }
            break;
        }

        // more file left
        size += BUFFER_SIZE;
        read = realloc(read, size + BUFFER_SIZE);
        // read[size] = '\0';
    }

    read[size] = '\0';

    fclose(inputFile);

    return read;
}
