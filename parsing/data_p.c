#include "parselib.h"
#include "korolib.h"
#include "common.h"
#include "commstruc_p.h"
#include "syntax.h"
#include "ret_types.h"
#include<stdlib.h>
#include<stdio.h>

parser* _datP = NULL;

void _data_koro(koroctx* kctx) {
    crBegin;
    #define _kdtype DataElement
    #define _krtype result
    ALLOCATE_DAT();
    crYield(posPlus);
    KMDAT(is_export) = KLAST != NULL;

    if (_datP == NULL) {
        _datP = strP("data");
    }
    crYield(_datP);
    crYield(KMDAT(size), int, bit8Or16, data);
    crYield(whitespace);

    crYield(KMDAT(name), char*, validIdentifier, data);
    cruYield(betWEq);
    crYield(opCurl);

    static ResArrD* rad;
    crYield(rad, ResArrD*, commaSepHex, data);
    cruYield(cloCurl);

    KMDAT(val_len) = rad->a_len;
    KMDAT(values) = malloc(rad->a_len * sizeof(Syntax*));
    for (int i = 0; i < rad->a_len; i++) {
        result* ri = rad->arr[i];
        KMDAT(values)[i] = ri->data;
        free(ri);
    }
    free(rad);

    Syntax* syn = create_syntax(
        DATA,
        DATA_ELEMENT_TYPE,
        KDDAT
    );
    result* res = create_result(SYNTAX_TYPE, syn);
    crReturn(res);
}

parser* create_data_parser() {
    return korop(&_data_koro, false);
}
