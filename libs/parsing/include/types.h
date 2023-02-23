#ifndef TYPES_HEADER
#define TYPES_HEADER

typedef enum Types {
    REGISTER,
    HEX_LITERAL,
    VARIABLE,
    ADDRESS,

    OP_PLUS,
    OP_MINUS,
    OP_MULTIPLY,
    OP_INT_DIVISION,

    BINARY_OPERATION,
    BRACKETED_EXPRESSION,
    SQUARE_BRACKET_EXPRESSION,

    INSTRUCTION,

    LABEL,
    DATA,
    CONSTANT,

    STRUCTURE,
    ARGUMENT_STRUCTURE,
    INTERPRET_AS_SINGLE,
    INTERPRET_AS_ARRAY,

    COMMENT,

    EXPR_ELEMENT,
} Types;

extern char* typeStrs[];

char* type_to_str(Types type);

#endif