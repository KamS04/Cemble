#ifndef RET_TYPES_HEADER
#define RET_TYPES_HEADER
#include<stdbool.h>
#include<stdint.h>
#include "syntax.h"
#include "instruction.h"

// Types.LABEL
typedef struct {
    char* name;
} SymbolicElement;
#define SYMBOLIC_ELEMENT_TYPE 5

// Types.DATA
typedef struct {
    int size;
    bool is_export;
    char* name;
    Syntax** values;
    int val_len;
} DataElement;
#define DATA_ELEMENT_TYPE 6

typedef struct {
    char* key;
    Syntax* val;
} SSynPair;
#define SSYN_PAIR_TYPE 7

// Types.STRUCTURE
typedef struct {
    bool is_export;
    char* name;
    SSynPair** members;
    int mem_len;
} StructureElement;
#define STRUCTURE_ELEMENT_TYPE 8

// Types.CONSTANT
typedef struct {
    bool is_export;
    char* name;
    Syntax* value;
} ConstantElement;
#define CONSTANT_ELEMENT_TYPE 9

// Types.INTERPRET_AS_SINGLE
typedef struct {
    char* structure;
    Syntax* symbol;
    char* property;
} InterpretSingle;
#define INTERPRET_SINGLE_TYPE 10

// Types.INTERPRET_AS_ARRAY
typedef struct {
    char* structure;
    Syntax* index;
    Syntax* symbol;
    char* property;
} InterpretArray;
#define INTERPRET_ARRAY_TYPE 11 

// Types.INSTRUCTION
typedef struct {
    Instruction* ins;
    Syntax** args;
    int args_s;
} InstructionItem;
#define INSTRUCTION_ITEM_TYPE 12

// Types.BINARY_OPERATION
typedef struct {
    char operation;
    Syntax* a;
    Syntax* b;
} BinaryOperation;
#define BINARY_OPERATION_TYPE 13

bool convert_ret_item_to_str(void* item, int type, char** out);

#endif