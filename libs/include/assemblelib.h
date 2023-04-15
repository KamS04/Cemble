#ifndef ASSEMBLE_LIB_HEADER
#define ASSEMBLE_LIB_HEADER

#include<stdbool.h>
#include<stdint.h>
#include<stdlib.h>

#include "parselib.h"
#include "oarrlist.h"
#include "instruction.h"

// Defines

// syntax.h
#define SYNTAX_TYPE 4

// log.h
#ifdef DEBUG
#define LOG(x)  _Pragma("GCC diagnostic push") \
                _Pragma("GCC diagnostic ignored \"-Wunused-value\"") \
                C_DEBUG_MODE && x; \
                _Pragma("GCC diagnostic pop")
#define assert(x) if (!(x)) { printf("Assertion %s, Failed\n", #x); exit(3); }
#else
#define LOG(x)
#define assert(x)
#endif

// ret_types.h
#define SYMBOLIC_ELEMENT_TYPE 5
#define DATA_ELEMENT_TYPE 6
#define SSYN_PAIR_TYPE 7
#define STRUCTURE_ELEMENT_TYPE 8
#define CONSTANT_ELEMENT_TYPE 9
#define INTERPRET_SINGLE_TYPE 10
#define INTERPRET_ARRAY_TYPE 11 
#define INSTRUCTION_ITEM_TYPE 12
#define BINARY_OPERATION_TYPE 13

// Typedefs

// types.h
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

// syntax.h

typedef struct Syntax {
    Types type;
    DataUnion value;
    int v_type;
} Syntax;

// ret_types.h
typedef struct SymbolicElement {
    char* name;
} SymbolicElement;

typedef struct DataElement {
    int size;
    bool is_export;
    char* name;
    Syntax** values;
    int val_len;
} DataElement;

typedef struct SSynPair {
    char* key;
    Syntax* val;
} SSynPair;

typedef struct StructureElement {
    bool is_export;
    char* name;
    SSynPair** members;
    int mem_len;
} StructureElement;

typedef struct ConstantElement {
    bool is_export;
    char* name;
    Syntax* value;
} ConstantElement;

typedef struct InterpretSingle {
    char* structure;
    Syntax* symbol;
    char* property;
} InterpretSingle;

typedef struct InterpretArray {
    char* structure;
    Syntax* index;
    Syntax* symbol;
    char* property;
} InterpretArray;

typedef struct InstructionItem {
    Instruction* ins;
    Syntax** args;
    int args_s;
} InstructionItem;

typedef struct BinaryOperation {
    char operation;
    Syntax* a;
    Syntax* b;
} BinaryOperation;

typedef struct LabelElement {
    char* name;
    bool is_export;
} LabelElement;
#define LABEL_ELEMENT_TYPE 14

// Member Declarations

// common.h
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

// types.h
extern char* typeStrs[];

// hl_parsers.h
extern parser* literal;
extern parser* memory;
extern parser* registerPtr;
extern parser* ampersand;

// expression.h
extern parser* expressionParser;
extern parser* expressionElement;
extern parser* operator;
extern parser* sqBrackParser;

// c_config.h
extern int C_DEBUG_MODE;

// commstruc_p.h
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

// instruction_p.h
extern parser** instructionParsers;

// Function Declarations

// comment.h
parser* create_comment_parser();

// common.h
parser* optionalWhitespaceSurrounded(parser* p);
parser* commaSeparated(parser* p);
parser* john_cena();
void init_common_ass_parsers();

// types.h
char* type_to_str(Types type);

// syntax.h
char* syntax_to_string(Syntax* syn, bool nl);
char* nsyntax_to_string(Syntax* syn);
Syntax* create_syntax(Types stype, int v_type, DataUnion value);
mapresult* syntax_mapper(result* res, DataUnion data);

// structure_p.h
parser* create_structure_parser();
parser* create_argument_structure_parser();

// ret_types.h
bool convert_ret_item_to_str(void* item, int type, char** out);

// interpret_as.h
parser* create_interpret_as_single();
parser* create_interpret_as_array();

// hl_parsers.h
void init_hl_parsers();

// expression.h
BinaryOperation* crebinop(Syntax* a, Syntax* b, char op);
int priority(char op);
BinaryOperation* binOpify(MAL elements, MAL operators);
parser* create_expression_parser();
parser* create_sqbrac_parser();

// data_p.h
parser* create_data_parser();

// constant_p.h
parser* create_constant_parser();

// commstruc_p.h
void init_commstruct_parsers();

// everything.h
parser* create_assembly_parser();
parser* single_assembly_parser();

// instruction_p.h
size_t instruction_size(InstructionType t);
parser* createInstructionParser(Instruction* ins);
parser* instructionParser();

#endif