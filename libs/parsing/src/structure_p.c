#include "parselib.h"
#include "korolib.h"
#include "common.h"
#include "commstruc_p.h"
#include "syntax.h"
#include "ret_types.h"
#include<stdlib.h>
#include<stdio.h>
#include<log.h>

parser* _strucP = NULL;
parser* _fstrucP = NULL; 
parser* _astrucP = NULL; 
parser* _argAstrucP = NULL; 

void _struc_koro(koroctx* kctx) {
    crBegin;
    #define _kdtype StructureElement
    #define _krtype result
    ALLOCATE_DAT();
    LOG(puts("start struct p"));
    crYield(posPlus);
    LOG(puts("struct +"));
    KMDAT(is_export) = KLAST != NULL;

    crYield(_strucP);
    LOG(puts("struct structure"));
    crYield(whitespace);
    crYield(KMDAT(name), char*, validIdentifier, data.ptr);
    LOG(puts("struct valid"));
    cruYield(optionalWhitespace);
    
    crYield(opCurl);
    LOG(puts("struct {"));

    #ifdef SINGLE_THREADED_ONLY
    static ResArrD* rad;
    crYield(rad, ResArrD*, commSepKVP, data.ptr);
    LOG(puts("struct commaSepKVP"));
    cruYield(cloCurl);
    LOG(puts("struct }"));

    KMDAT(mem_len) = rad->a_len;
    KMDAT(members) = malloc(rad->a_len * sizeof(Syntax*));
    for (int i = 0; i < rad->a_len; i++) {
        result* ri = rad->arr[i];
        KMDAT(members)[i] = ri->data;
        free(ri);
    }
    free(rad);
    #else
    // evile, result is actually a ResArrD* pointer, but we're pretending its a SSynPair**
    // in order to save in the context/data
    // truly vile
    crYield(KMDAT(members), SSynPair**, commSepKVP, data.ptr);
    LOG(puts("struct commaSepKVP"));
    cruYield(cloCurl); // TODO What happens when we fail here ?????
    // TODO if a failure occurs at the cloCurl, then the koroutine loop quits
    // TODO and then the current koroctx is deallocated alongside the data inside it
    // TODO (this is not implemented yet hence why no errors are raised) BUT
    // TODO when that feature is enabled, then we need a way to get around this
    // TODO the best idea seems to be to make cloCurl a possibly parser
    // TODO then if it does even if it does fail the control will return to the koroutine and 
    // TODO a actual failure can be triggered from here, AFTER a deallocation of the EVILE pointer magic
    LOG(puts("struct }"));

    // Remember KMDAT(values) is a SSynPair** pointer, but we saved a ResArrD* pointer inside it
    // be oh so fuckiiiiiing careful bro
    // once again truly vile
    ResArrD* rad = (ResArrD*)KMDAT(members);
    KMDAT(mem_len) = rad->a_len;
    KMDAT(members) = malloc(rad->a_len * sizeof(Syntax*));
    for (int i = 0; i < rad->a_len; i++) {
        result* ri = rad->arr[i].ptr;
        KMDAT(members)[i] = ri->data.ptr;
        free(ri);
    }
    free(rad);
    #endif

    Syntax* syn = create_syntax(
        STRUCTURE,
        STRUCTURE_ELEMENT_TYPE,
        (DataUnion){ .ptr = KDDAT }
    );
    result* res = create_result(SYNTAX_TYPE, (DataUnion){ .ptr = syn });
    crReturn(res);
}

void _args_struc_koro(koroctx* kctx) {
    crBegin;
    #define _kdtype StructureElement
    #define _krtype result
    ALLOCATE_DAT();
    LOG(puts("start struct p"));
    crYield(posPlus);
    LOG(puts("struct +"));
    KMDAT(is_export) = KLAST != NULL;

    crYield(_astrucP);
    LOG(puts("struct structure"));
    crYield(whitespace);
    crYield(KMDAT(name), char*, validIdentifier, data.ptr);
    LOG(puts("struct valid"));
    cruYield(optionalWhitespace);
    
    crYield(opCurl);
    LOG(puts("struct {"));

    #ifdef SINGLE_THREADED_ONLY
    static ResArrD* rad;
    crYield(rad, ResArrD*, commSepKVP, data.ptr);
    LOG(puts("struct commaSepKVP"));
    cruYield(cloCurl);
    LOG(puts("struct }"));

    KMDAT(mem_len) = rad->a_len;
    KMDAT(members) = malloc(rad->a_len * sizeof(Syntax*));
    for (int i = 0; i < rad->a_len; i++) {
        result* ri = rad->arr[i];
        KMDAT(members)[i] = ri->data;
        free(ri);
    }
    free(rad);
    #else
    // evile, result is actually a ResArrD* pointer, but we're pretending its a SSynPair**
    // in order to save in the context/data
    // truly vile
    crYield(KMDAT(members), SSynPair**, commSepKVP, data.ptr);
    LOG(puts("struct commaSepKVP"));
    cruYield(cloCurl); // TODO What happens when we fail here ?????
    // TODO if a failure occurs at the cloCurl, then the koroutine loop quits
    // TODO and then the current koroctx is deallocated alongside the data inside it
    // TODO (this is not implemented yet hence why no errors are raised) BUT
    // TODO when that feature is enabled, then we need a way to get around this
    // TODO the best idea seems to be to make cloCurl a possibly parser
    // TODO then if it does even if it does fail the control will return to the koroutine and 
    // TODO a actual failure can be triggered from here, AFTER a deallocation of the EVILE pointer magic
    LOG(puts("struct }"));

    // Remember KMDAT(values) is a SSynPair** pointer, but we saved a ResArrD* pointer inside it
    // be oh so fuckiiiiiing careful bro
    // once again truly vile
    ResArrD* rad = KMDAT(members);
    KMDAT(mem_len) = rad->a_len;
    KMDAT(members) = malloc(rad->a_len * sizeof(Syntax*));
    for (int i = 0; i < rad->a_len; i++) {
        result* ri = rad->arr[i].ptr;
        KMDAT(members)[i] = ri->data.ptr;
        free(ri);
    }
    free(rad);
    #endif

    Syntax* syn = create_syntax(
        ARGUMENT_STRUCTURE,
        STRUCTURE_ELEMENT_TYPE,
        (DataUnion){ .ptr = KDDAT }
    );
    result* res = create_result(SYNTAX_TYPE, (DataUnion){ .ptr = syn });
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
