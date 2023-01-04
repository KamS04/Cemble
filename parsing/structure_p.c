#include "parselib.h"
#include "korolib.h"
#include "common.h"
#include "commstruc_p.h"
#include "syntax.h"
#include "ret_types.h"
#include<stdlib.h>
#include<stdio.h>

parser* _strucP = NULL;
parser* _fstrucP = NULL; 
parser* _astrucP = NULL; 
parser* _argAstrucP = NULL; 

void _struc_koro(koroctx* kctx) {
    crBegin;
    #define _kdtype StructureElement
    #define _krtype result
    ALLOCATE_DAT();
    crYield(posPlus);
    KMDAT(is_export) = KLAST != NULL;

    crYield(_strucP);
    crYield(whitespace);
    crYield(KMDAT(name), char*, validIdentifier, data);
    cruYield(optionalWhitespace);
    
    crYield(opCurl);
    static ResArrD* rad;
    crYield(rad, ResArrD*, commSepKVP, data);
    cruYield(cloCurl);

    KMDAT(mem_len) = rad->a_len;
    KMDAT(members) = malloc(rad->a_len * sizeof(Syntax*));
    for (int i = 0; i < rad->a_len; i++) {
        result* ri = rad->arr[i];
        KMDAT(members)[i] = ri->data;
        free(ri);
    }
    free(rad);

    Syntax* syn = create_syntax(
        STRUCTURE,
        STRUCTURE_ELEMENT_TYPE,
        KDDAT
    );
    result* res = create_result(SYNTAX_TYPE, syn);
    crReturn(res);
}

void _args_struc_koro(koroctx* kctx) {
    crBegin;
    #define _kdtype StructureElement
    #define _krtype result
    ALLOCATE_DAT();
    crYield(posPlus);
    KMDAT(is_export) = KLAST != NULL;

    crYield(_astrucP);
    crYield(whitespace);
    crYield(KMDAT(name), char*, validIdentifier, data);
    cruYield(optionalWhitespace);

    crYield(opCurl);
    static ResArrD* rad;
    crYield(rad, ResArrD*, commSepKVP, data);
    cruYield(cloCurl);

    KMDAT(mem_len) = rad->a_len;
    KMDAT(members) = malloc(rad->a_len * sizeof(Syntax*));
    for (int i = 0; i < rad->a_len; i++) {
        result* ri = rad->arr[i];
        KMDAT(members)[i] = ri->data;
        free(ri);
    }
    free(rad);

    Syntax* syn = create_syntax(
        ARGUMENT_STRUCTURE,
        STRUCTURE_ELEMENT_TYPE,
        KDDAT
    );
    result* res = create_result(SYNTAX_TYPE, syn);
    crReturn(res);
}

parser* create_structure_parser() {
    if (_fstrucP == NULL)  {
        _strucP = strP("structure");
        _fstrucP = korop(&_struc_koro, false);
    }
    return _fstrucP;
}

parser* create_argument_structure_parser() {
    if (_astrucP == NULL) {
        _astrucP = strP("arguments");
        _argAstrucP = korop(&_args_struc_koro, false);
    }
    return _argAstrucP;
}
