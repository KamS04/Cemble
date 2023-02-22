#include "korolib.h"
#include "parselib.h"
#include "common.h"
#include "syntax.h"

parser* _euEOL = NULL;
parser* _euEOLCarr[2];
parser* _euEOLC;

void _commentKoro(koroctx* kctx) {
    #define _krtype result
    crBegin;
    crYield(optionalWhitespace);
    crYield(semiColon);
    crYield(KDDAT, char*, _euEOL, data);
    cruYield(optionalWhitespace);

    Syntax* syn = create_syntax(COMMENT, STRING, KDDAT);
    result* res = create_result(SYNTAX_TYPE, syn);
    crReturn(res);
}

parser* create_comment_parser() {
    if (_euEOL == NULL) {
        _euEOLCarr[0] = endOfInput;
        _euEOLCarr[1] = charP('\n');
        _euEOLC = choice(_euEOLCarr, 2);
        _euEOL = everythingUntil(_euEOLC, true);
    }

    return korop(&_commentKoro, false);
}
