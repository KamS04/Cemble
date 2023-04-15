#include "commstruc_p.h"
#include "parselib.h"
#include "common.h"
#include<stdio.h>

parser* betWEq = NULL;

parser* opCurl = NULL;
parser* cloCurl = NULL;

parser* commaSepHex = NULL;

parser* opAng;
parser* cloAng;

parser* opSt;
parser* cloSt;

parser* opBr;
parser* cloBr;

parser* arrBrack;

parser* dot;

parser* bit8Or16;

parser* _b8o16[2];

mapresult* _b8o16_mapper(result* re_in, void* data) {
    mapresult* ti = malloc(sizeof(mapresult));
    ti->dealloc_old = true;
    int si;
    sscanf(re_in->data.ptr, "%d", &si);
    ti->res = create_result(INTEGER, (DataUnion){ .in = si });
    return ti;
}

void init_commstruct_parsers() {
    betWEq = optionalWhitespaceSurrounded(charP('='));

    opBr = then(charP('('), optionalWhitespace, false);
    cloBr = optionalWhitespaceSurrounded(charP(')'));
    opCurl = then(charP('{'), optionalWhitespace, false);
    cloCurl = optionalWhitespaceSurrounded(charP('}'));

    commaSepHex = commaSeparated(hexLiteral);

    _b8o16[0] = strP("8");
    _b8o16[1] = strP("16");
    bit8Or16 = map(choice(_b8o16, 2), _b8o16_mapper, false, (DataUnion){ .ptr = NULL });

    opAng = charP('<');
    cloAng = charP('>');
    dot = charP('.');
    arrBrack = strP("[]");
    opSt = charP('[');
    cloSt = charP(']');
}
