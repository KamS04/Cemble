#include "parselib.h"
#include "common.h"
#include "expression.h"

parser* literal;
parser* _litArr[2];
parser* memory;
parser* _memArr[2];
parser* registerPtr;
parser* ampersand;

void init_hl_parsers() {
    ampersand = charP('&');
    _litArr[0] = hexLiteral;
    _litArr[1] = create_sqbrac_parser();
    literal = choice(_litArr, 2);
    _memArr[0] = address;
    _memArr[1] = then(ampersand, create_sqbrac_parser(), false);
    memory = choice(_memArr, 2);
    registerPtr = then(ampersand, registers, false);
}
