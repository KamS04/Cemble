#include "parselib.h"
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include "syntax.h"
#include "types.h"
#include "common.h"
#include "korolib.h"
#include "ret_types.h"
// #include "mutarr.h"

parser* registers;
parser* hexDigit;
parser* hexLiteral;
parser* address;
parser* validIdentifier;
parser* variable;
parser* peek;
parser* label;
parser* comma;
parser* colon;
parser* keyValP;
parser* commSepKVP;
parser* semiColon;
parser* nLineOrEnd;

parser* _nloearr[2];

parser* optionalWhitespaceSurrounded(parser* p) {
    return between(optionalWhitespace, p, optionalWhitespace);
}

parser* commaSeparated(parser* p) {
    return sepBy(p, comma);
}

void _key_val(koroctx* kctx) {
    crBegin;
    #define _kdtype SSynPair
    #define _krtype result
    ALLOCATE_DAT();
    crYield(optionalWhitespace);
    crYield(KMDAT(key), char*, validIdentifier, data.ptr);
    cruYield(colon);

    crYield(KMDAT(val), Syntax*, hexLiteral, data.ptr);

    cruYield(optionalWhitespace);

    result* res = create_result(SSYN_PAIR_TYPE, (DataUnion){ .ptr = KDDAT });
    // char* ss;
    // convert_ret_item_to_str(KDDAT, SSYN_PAIR_TYPE, &ss);
    crReturn(res);
}

/*
acu, ip
r1,  r2, r3, r4
r5,  r6, r7, r8
fp, sp
*/

mapresult* labelMapper(result* r, DataUnion data) {
    mapresult* mr = malloc(sizeof(mapresult));
    mr->dealloc_old = true;
    ResArrD* rad = r->data.ptr;
    char* lname = ((result*)(rad->arr[0].ptr))->data.ptr;
    // char* lname = ((result**)rad->arr)[0]->data;
    free(rad->arr[0].ptr);
    rad->arr[0].ptr = NULL;
    mr->res = create_result(
        SYNTAX_TYPE,
        (DataUnion){ .ptr = create_syntax(
            LABEL,
            STRING,
            (DataUnion){ .ptr = lname }
        )}
    );
    return mr;
}

void init_common_ass_parsers() {
    comma = optionalWhitespaceSurrounded(charP(','));
    colon = optionalWhitespaceSurrounded(charP(':'));

    parser** rs = malloc( 14 * sizeof(parser*) );
    parser* trs[] = {
        strP("ip"), strP("acu"),
        strP("r1"), strP("r2"), strP("r3"), strP("r4"),
        strP("r5"), strP("r6"), strP("r7"), strP("r8"),
        strP("sp"), strP("fp"), strP("mb"), strP("im")
    };
    memcpy(rs, trs, 14 * sizeof(parser*));
    registers = map(choice(rs, 14), &syntax_mapper, false, (DataUnion){ .in = (int)REGISTER });

    hexDigit = regexP("^[0-9a-fA-F]");
    hexLiteral = map(regexP("^\\$[0-9a-fA-F]+"), &syntax_mapper, false, (DataUnion){ .in = (int)HEX_LITERAL });

    address = map(regexP("^&[0-9a-fA-F]+"), &syntax_mapper, false, (DataUnion){ .in = (int)ADDRESS });

    validIdentifier = regexP("^[a-zA-Z_][a-zA-Z0-9_]*");

    variable = map(then(charP('!'), validIdentifier, false), &syntax_mapper, false, (DataUnion){ .in = (int)VARIABLE });

    peek = lookAhead(anyChar);

    parser** l = malloc( 3 * sizeof(parser*) );
    l[0] = validIdentifier;
    l[1] = charP(':');
    l[2] = optionalWhitespace;
    // Get first item out of array for label
    label = map(sequenceOf(l, 3), &labelMapper, false, (DataUnion){ .ptr = NULL });

    keyValP = korop(&_key_val, false);
    commSepKVP = commaSeparated(keyValP);
    semiColon = charP(';');

    _nloearr[0] = charP('\n');

    
    _nloearr[1] = endOfInput;
    nLineOrEnd = choice(_nloearr, 2);
}
