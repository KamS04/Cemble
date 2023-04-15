#ifndef COMMON_ASS_HEADER
#define COMMON_ASS_HEADER

extern parser* registers;
extern parser* hexDigit;
extern parser* hexLiteral;
extern parser* address;
extern parser* validIdentifier;
extern parser* variable;
extern parser* peek;
extern parser* label;
extern parser* comma;
extern parser* keyValP;
extern parser* commSepKVP;
extern parser* semiColon;
extern parser* nLineOrEnd;
extern parser* posPlus;

parser* optionalWhitespaceSurrounded(parser* p);
parser* commaSeparated(parser* p);

void init_common_ass_parsers();
#endif
