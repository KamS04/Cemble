#ifndef SYNTAX_HEADER
#define SYNTAX_HEADER
#include "types.h"
#include "parselib.h"
#include<stdbool.h>
#define SYNTAX_TYPE 4

typedef struct Syntax {
    Types type;
    DataUnion value;
    int v_type;
} Syntax;

char* syntax_to_string(Syntax* syn, bool nl);
char* nsyntax_to_string(Syntax* syn);

Syntax* create_syntax(Types stype, int v_type, DataUnion value);

mapresult* syntax_mapper(result* res, DataUnion data);

#endif