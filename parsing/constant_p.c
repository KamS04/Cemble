#include "parselib.h"
#include "korolib.h"
#include "common.h"
#include "syntax.h"
#include "ret_types.h"
#include<stdlib.h>
#include<stdio.h>
#include "commstruc_p.h"

parser* _consStr = NULL;

void _constant_coro(koroctx* kctx) {
    crBegin;
    #define _kdtype ConstantElement
    #define _krtype result
    ALLOCATE_DAT();

    void* q;
    crYield(q, void*, posPlus);
    KMDAT(is_export) = q != NULL;
    
    if (_consStr == NULL) {
        _consStr = strP("constant");
    }
    crYield(_consStr);
    crYield(whitespace);

    crYield(KMDAT(name), char*, validIdentifier, data);
    
    cruYield(betWEq);

    crYield(KMDAT(value), Syntax*, hexLiteral, data);

    cruYield(optionalWhitespace);

    Syntax* s = create_syntax(
        CONSTANT,
        CONSTANT_ELEMENT_TYPE,
        KDDAT
    );
    result* res = create_result(SYNTAX_TYPE, s);
    crReturn(res);
}


parser* create_constant_parser() {
    return korop(_constant_coro, false);
}