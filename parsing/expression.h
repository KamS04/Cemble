#ifndef EXPRESSION_PARSER_HEADER
#define EXPRESSION_PARSER_HEADER

#include "ret_types.h"
#include "syntax.h"
// #include "omutarr.h"
#include "oarrlist.h"

extern parser* expressionParser;
extern parser* expressionElement;
extern parser* operator;
extern parser* sqBrackParser;

BinaryOperation* crebinop(Syntax* a, Syntax* b, char op);

int priority(char op);

BinaryOperation* binOpify(MAL elements, MAL operators);

parser* create_expression_parser();
parser* create_sqbrac_parser();

#endif