@echo off

gcc -c common.c constant_p.c ^
    syntax.c types.c ret_types.c c_config.c ^
    commstruc_p.c structure_p.c interpret_as.c ^
    data_p.c expression.c hl_parsers.c comment.c everything.c ^
    instruction_p.c ^
    -ggdb -DDEBUG

ar rcs %* common.o constant_p.o ^
    syntax.o types.o ret_types.o c_config.o ^
    commstruc_p.o structure_p.o interpret_as.o ^
    instruction_p.o ^
    data_p.o expression.o hl_parsers.o comment.o everything.o

del *.o
