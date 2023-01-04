#ifndef HL_PARSERS_HEADER
#define HL_PARSERS_HEADER

extern parser* literal;
extern parser* memory;
extern parser* registerPtr;
extern parser* ampersand;

void init_hl_parsers();

#endif