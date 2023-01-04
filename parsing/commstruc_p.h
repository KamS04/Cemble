#ifndef COMM_STRUCT_PARSER
#define COMM_STRUCT_PARSER

#include "parselib.h"

extern parser* posPlus;
extern parser* betWEq;

extern parser* opCurl;
extern parser* cloCurl;

extern parser* commaSepHex;

extern parser* bit8Or16;

extern parser* opAng;
extern parser* cloAng;

extern parser* opSt;
extern parser* cloSt;

extern parser* opBr;
extern parser* cloBr;

extern parser* arrBrack;

extern parser* dot;

void init_commstruct_parsers();

#endif