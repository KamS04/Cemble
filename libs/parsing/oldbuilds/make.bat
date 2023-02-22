@echo off

gcc %* common.c constant_p.c putils.c ^
    syntax.c types.c ret_types.c ^
    c_config.c commstruc_p.c structure_p.c ^
    interpret_as.c data_p.c expression.c oarrlist.c ^
    hl_parsers.c comment.c everything.c instruction_p.c ^
    -L../libs -ldparselib -DDEBUG
