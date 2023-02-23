#include "parselib.h"
#include "korolib.h"
#include "common.h"
#include "commstruc_p.h"
#include "syntax.h"
#include "ret_types.h"
#include<stdlib.h>
#include<stdio.h>
#include "log.h"

parser* _datP = NULL;

void _data_koro(koroctx* kctx) {
    crBegin;
    #define _kdtype DataElement
    #define _krtype result
    LOG(puts("reached data koro"));
    ALLOCATE_DAT();
    crYield(posPlus);
    LOG(puts("data koro +"));
    KMDAT(is_export) = KLAST != NULL;

    if (_datP == NULL) {
        _datP = strP("data");
    }
    crYield(_datP);
    crYield(KMDAT(size), int, bit8Or16, data.in);
    crYield(whitespace);
    LOG(puts("data koro: data_size"));

    crYield(KMDAT(name), char*, validIdentifier, data.ptr);
    LOG(puts("data koro: valid_iden"));
    cruYield(betWEq);
    LOG(puts("data koro: betweq"));
    crYield(opCurl);
    LOG(puts("data koro: opcurl"));

    #ifdef SINGLE_THREADED_ONLY
    static ResArrD* rad;
    crYield(rad, ResArrD*, commaSepHex, data.ptr);
    LOG(puts("data koro: rad bro")); 
    cruYield(cloCurl);

    KMDAT(val_len) = rad->a_len;
    KMDAT(values) = malloc(rad->a_len * sizeof(Syntax*));
    for (int i = 0; i < rad->a_len; i++) {
        result* ri = rad->arr[i];
        KMDAT(values)[i] = ri->data;
        free(ri);
    }
    free(rad);
    #else
    // evile, result is actually a ResArrD* pointer, but we're pretending its a Syntax**
    // in order to save in the context/data
    // truly vile
    crYield(KMDAT(values), Syntax**, commaSepHex, data.ptr);
    LOG(puts("data koro: rad bro")); 
    cruYield(cloCurl);
    // TODO same problem as the other one refer to my other rant

    // Remember KMDAT(values) is a Syntax** pointer, but we saved a ResArrD* pointer inside it
    // be oh so fuckiiiiiing careful bro
    // once again truly vile
    ResArrD* rad = (ResArrD*)KMDAT(values);
    KMDAT(val_len) = rad->a_len;
    KMDAT(values) = malloc(rad->a_len * sizeof(Syntax**));
    for (int i = 0; i < rad->a_len; i++) {
        result* ri = rad->arr[i].ptr;
        KMDAT(values)[i] = ri->data.ptr;
        free(ri);
    }
    free(rad);
    #endif

    Syntax* syn = create_syntax(
        DATA,
        DATA_ELEMENT_TYPE,
        (DataUnion){ .ptr = KDDAT }
    );
    result* res = create_result(SYNTAX_TYPE, (DataUnion){ .ptr = syn });
    crReturn(res);
}

parser* create_data_parser() {
    return korop(_data_koro, false);
}
