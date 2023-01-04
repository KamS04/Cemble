#!/bin/bash

libpath="$(realpath .)/libs"

gcc $* commandline.c oarrlist.c setup.c korolib.c assembler.c writer.c \
    reader.c sort.c map.c encoder.c encode.c \
    ltiming.c lfilereader.c formats.c instruction.c \
    -L$libpath -llassemblelib -llparselib -lm
