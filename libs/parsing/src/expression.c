#include "parselib.h"
#include "korolib.h"
#include "oarrlist.h"
#include "commstruc_p.h"
#include "interpret_as.h"
#include "common.h"
#include "syntax.h"
#include "types.h"
#include "ret_types.h"
#include<stdio.h>

parser* expressionParser = NULL;
parser* _unexeof;
parser* expressionElement = NULL;
parser* _exEleArr[4];
parser* operator;
parser* _opArr[4];
parser* sqBrackParser = NULL;

typedef enum ExprState {
    OPEN_BRACKET,
    OPERATOR_OR_CLOSE_BRACKET,
    ELEMENT_OR_OPEN_BRACKET
} ExprState;

BinaryOperation* crebinop(Syntax* a, Syntax* b, char op) {
    BinaryOperation* bo = malloc(sizeof(BinaryOperation));
    bo->a = a;
    bo->b = b;
    bo->operation = op;
    return bo;
}

int priority(char op) {
    switch (op) {
        case '/': return 3;
        case '*': return 2;
        case '+': return 1;
        case '-': return 0;
    }
    return -1;
}

// elements@ArrayList<Syntax*>, operators@ArrayList<char>
BinaryOperation* binOpify(MAL elements, MAL operators) {
    BinaryOperation* cBO = crebinop(APL(elements)[0], APL(elements)[1], ACL(operators)[0]);
    int cEleIdx = 2;
    int cPrio = priority(cBO->operation);

    for (int i = 1; i < SIZEOF(operators); i++) {
        char nOp = ACL(operators)[i];
        Syntax* nEle = APL(elements)[cEleIdx++];

        if (priority(nOp) > cPrio) {
            cBO->b = create_syntax(
                BINARY_OPERATION,
                BINARY_OPERATION_TYPE,
                (DataUnion){ .ptr = crebinop(
                    cBO->b,
                    nEle,
                    nOp
                )}
            );
        } else {
            cBO = crebinop(
                create_syntax(
                    BINARY_OPERATION,
                    BINARY_OPERATION_TYPE,
                    (DataUnion){ .ptr = cBO }
                ),
                nEle,
                nOp
            );
        }
    }

    return cBO;
}

/*
    elements@ArrayList<Syntax*>
    operators@ArrayList<char>
*/
typedef struct ExprKData {
    ExprState state;
    MAL elements;
    MAL operators;
    char next_char;
} ExprKData;
void _exprKoro(koroctx* kctx) {
    #define _kdtype ExprKData
    #define _krtype result
    Syntax* nextItem;

    crBegin;
    ALLOCATE_DAT();
    KMDAT(state) = ELEMENT_OR_OPEN_BRACKET;
    KMDAT(elements) = AllocatePtrAL(10, 10, 0, NULL);
    KMDAT(operators) = AllocateCharAL(10, 10, 1, '\0');

    while (true) {
        crYield(KMDAT(next_char), char, peek, data.ch);

        if (KMDAT(state) == OPEN_BRACKET) {
            crYield(opBr);
            crYield(nextItem, Syntax*, expressionParser, data.ptr);
            PAPPEND(KMDAT(elements), nextItem);
            cruYield(cloBr);
            KMDAT(state) = OPERATOR_OR_CLOSE_BRACKET;
        } else if (KMDAT(state) == ELEMENT_OR_OPEN_BRACKET) {
            if (KMDAT(next_char) == ')') {
                crYield(_unexeof);
            }
            if (KMDAT(next_char) == '(') {
                KMDAT(state) = OPEN_BRACKET;
            } else {
                crYield(nextItem, Syntax*, expressionElement, data.ptr);
                PAPPEND(KMDAT(elements), nextItem);
                cruYield(optionalWhitespace);
                KMDAT(state) = OPERATOR_OR_CLOSE_BRACKET;
            }
        } else if (KMDAT(state) == OPERATOR_OR_CLOSE_BRACKET) {
            if (KMDAT(next_char) == ')' || KMDAT(next_char) == ']') {
                break;
            }
            crYield(nextItem, Syntax*, operator, data.ptr);
            CAPPEND(KMDAT(operators), (char)nextItem->value.ch);
            cruYield(optionalWhitespace);
            KMDAT(state) = ELEMENT_OR_OPEN_BRACKET;
        }
    }

    shrink_to_needed(KMDAT(elements));
    if (SIZEOF(KMDAT(operators))) {
        shrink_to_needed(KMDAT(operators));
    }

    if (SIZEOF(KMDAT(elements)) == 1) {
        nextItem = APL(KMDAT(elements))[0];
    } else {
        BinaryOperation* bo = binOpify(KMDAT(elements), KMDAT(operators));
        nextItem = create_syntax(
            BINARY_OPERATION,
            BINARY_OPERATION_TYPE,
            (DataUnion){ .ptr = bo }
        );
    }

    free(AL(KMDAT(elements)));
    free(KMDAT(elements));
    free(AL(KMDAT(operators)));
    free(KMDAT(operators));

    result* res = create_result(SYNTAX_TYPE, (DataUnion){ .ptr = nextItem });
    crReturn(res);
}

void _inieph() {
    _unexeof = fail("bracketedExpr: Unexpected end of expression");
    _exEleArr[0] = create_interpret_as_single();
    _exEleArr[1] = create_interpret_as_array();
    _exEleArr[2] = hexLiteral;
    _exEleArr[3] = variable;
    expressionElement = choice(_exEleArr, 4);

    _opArr[0] = map( charP('+'), syntax_mapper, false, (DataUnion){ .in = (int)OP_PLUS });
    _opArr[1] = map( charP('-'), syntax_mapper, false, (DataUnion){ .in = (int)OP_MINUS });
    _opArr[2] = map( charP('*'), syntax_mapper, false, (DataUnion){ .in = (int)OP_MULTIPLY });
    _opArr[3] = map( charP('/'), syntax_mapper, false, (DataUnion){ .in = (int)OP_INT_DIVISION });
    operator = choice(_opArr, 4);
}

parser* create_expression_parser() {
    if (expressionParser == NULL) {
        _inieph();
        expressionParser = korop(&_exprKoro, false);
    }
    return expressionParser;
}

parser* create_sqbrac_parser() {
    if (sqBrackParser == NULL) {
        sqBrackParser = map(
            between(opSt, create_expression_parser(), cloSt),
            &syntax_mapper, false, (DataUnion){ .in = SQUARE_BRACKET_EXPRESSION }
        );
    }
    return sqBrackParser;
}
