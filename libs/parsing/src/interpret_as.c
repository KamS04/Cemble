#include "parselib.h"
#include "korolib.h"
#include "common.h"
#include "commstruc_p.h"
#include "ret_types.h"
#include "types.h"

parser* _chSt = NULL;
parser* _cpit[2];


void _iniIntH() {
    if (_chSt == NULL) {
        _cpit[0] = variable;
        _cpit[1] = hexLiteral;
        _chSt = choice(_cpit, 2);
    }
}

void _intAsS(koroctx* kctx) {
    crBegin;
    #define _kdtype InterpretSingle
    #define _krtype result
    ALLOCATE_DAT();
    crYield(opAng);
    crYield(KMDAT(structure), char*, validIdentifier, data.ptr);
    cruYield(cloAng);

    crYield(KMDAT(symbol), Syntax*, _chSt, data.ptr);
    cruYield(dot);
    crYield(KMDAT(property), char*, validIdentifier, data.ptr);

    cruYield(optionalWhitespace);

    Syntax* syn = create_syntax(
        INTERPRET_AS_SINGLE,
        INTERPRET_SINGLE_TYPE,
        (DataUnion){ .ptr = KDDAT }
    );
    result* res = create_result(SYNTAX_TYPE, (DataUnion){ .ptr = syn });
    crReturn(res);
}

void _intAsA(koroctx* kctx) {
    #define _kdtype InterpretArray
    #define _krtype result
    crBegin;
    ALLOCATE_DAT();
    crYield(opAng);
    crYield(KMDAT(structure), char*, validIdentifier, data.ptr);
    cruYield(arrBrack);
    crYield(cloAng);

    crYield(KMDAT(symbol), Syntax*, _chSt, data.ptr);
    crYield(opSt);
    crYield(KMDAT(index), Syntax*, _chSt, data.ptr);
    cruYield(cloSt);
    crYield(dot);

    crYield(KMDAT(property), char*, validIdentifier, data.ptr);
    cruYield(optionalWhitespace);

    Syntax* syn = create_syntax(
        INTERPRET_AS_ARRAY,
        INTERPRET_ARRAY_TYPE,
        (DataUnion){ .ptr = KDDAT }
    );
    result* res = create_result(SYNTAX_TYPE, (DataUnion){ .ptr = syn });
    crReturn(res);
}

parser* create_interpret_as_single() {
    _iniIntH();
    return korop(_intAsS, false);
}

parser* create_interpret_as_array() {
    _iniIntH();
    return korop(_intAsA, false);
}
