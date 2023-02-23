#include<stdint.h>
#include<math.h>
#include<stdio.h>
#include "assemblelib.h"
#include "map.h"
#include "encoder.h"
#include "encode.h"
#include "string.h"

uint16_t hex_to_int16(char* hex) {
    int le;
    for (le = 1; le <= 4; le++) {
        if (hex[le] == '\0') {
            break;
        }
    }

    uint16_t t[4] = { 0, 0, 0, 0 };
    int id = 4 - le;
    char c;
    for (int i = 0; i < le; i++) {
        c = hex[i];
        if ('a' <= c) {
            t[id++] = c - 'W'; // 'a' - 10 == 'W'
        } else if ('A' <= c) {
            t[id++] = c - '7'; // 'A' - 10 == '7'
        } else {
            t[id++] = c - '0';
        }
    }
    return t[0] << 12 | t[1] << 8 | t[2] << 4 | t[3];
}

bool random_boolean;
uint16_t evaluate_node(AssemblyContext* asmCon, Syntax* syn, Map symbols, Map structures) {
    return evaluate_node_core(asmCon, syn, symbols, structures, &random_boolean);
}

uint16_t evaluate_node_core(AssemblyContext* asmCon, Syntax* syn, Map symbols, Map structures, bool* borke) {
    switch (syn->type) {
        case VARIABLE:
            assert(syn->v_type == STRING)
            if (strcmp(syn->value.ptr, "CLINE") == 0) {
                if (asmCon == NULL) {
                    *borke = true;
                    return 0;
                }
                return asmCon->cAddr;
            }
            return gm_int(symbols, syn->value.ptr);
        case INTERPRET_AS_SINGLE: {
            assert(syn->v_type == INTERPRET_SINGLE_TYPE)
            InterpretSingle* is = syn->value.ptr;
            StructureComp* structure = gm_ptr(structures, is->structure);
            StructMember* member = gm_ptr(structure->members, is->property);
            uint16_t symbol = evaluate_node(asmCon, is->symbol, symbols, structures);
            return symbol + member->offset;
        }
        case INTERPRET_AS_ARRAY: {
            assert(syn->v_type == INTERPRET_ARRAY_TYPE)
            InterpretArray* ia = syn->value.ptr;
            StructureComp* structure = gm_ptr(structures, ia->structure);
            StructMember* member = gm_ptr(structure->members, ia->property);
            uint16_t symbol = evaluate_node(asmCon, ia->symbol, symbols, structures);
            uint16_t index = evaluate_node(asmCon, ia->index, symbols, structures);
            return symbol + ( index * structure->totalSize ) + member->offset;
        }
        case ADDRESS:
        case HEX_LITERAL:
            assert(syn->v_type == STRING)
            return hex_to_int16(&((char*)syn->value.ptr)[1]);
        
        case SQUARE_BRACKET_EXPRESSION:
            assert(syn->v_type == SYNTAX_TYPE)
            return evaluate_node(asmCon, syn->value.ptr, symbols, structures);
        
        case BINARY_OPERATION: {
            assert(syn->v_type == BINARY_OPERATION_TYPE)
            BinaryOperation* bo = syn->value.ptr;
            uint16_t a = evaluate_node(asmCon, bo->a, symbols, structures);
            uint16_t b = evaluate_node(asmCon, bo->b, symbols, structures);
            if (bo->operation == '+') {
                return a + b;
            } else if (bo->operation == '-') {
                return a - b;
            } else if (bo->operation == '*') {
                return a * b;
            } else {
                return a / b;
            }
        }
        default:
            printf("Unsupported node type for evaluate_node: %d", syn->type);
            if (syn->type < 21) {
                printf("%s", typeStrs[syn->type]);
            }
            puts("");
            exit(3);
    }
}

void encodeInstruction(uint8_t* mCode, AssemblyContext* asmCon, InstructionItem* iex) {
    mCode[asmCon->cAddr++] = iex->ins->opcode;
    switch (iex->ins->type) {
        case litReg:
        case memReg: {
            encodeLitOrMem(mCode, asmCon, iex->args[0]);
            encodeReg(mCode, asmCon, iex->args[1]);
            break;
        }
        case regLit:
        case regMem: {
            encodeReg(mCode, asmCon, iex->args[0]);
            encodeLitOrMem(mCode, asmCon, iex->args[1]);
            break;
        }
        case regLit8: {
            encodeReg(mCode, asmCon, iex->args[0]);
            encodeLit8OrMem(mCode, asmCon, iex->args[1]);
            break;
        }
        case regReg:
        case regPtrReg:
        case regRegPtr: {
            encodeReg(mCode, asmCon, iex->args[0]);
            encodeReg(mCode, asmCon, iex->args[1]);
            break;
        }
        case litMem: {
            encodeLitOrMem(mCode, asmCon, iex->args[0]);
            encodeLitOrMem(mCode, asmCon, iex->args[1]);
            break;
        }
        case lit8Mem: {
            encodeLit8OrMem(mCode, asmCon, iex->args[0]);
            encodeLitOrMem(mCode, asmCon, iex->args[1]);
            break;
        }
        case litOffReg: {
            encodeLitOrMem(mCode, asmCon, iex->args[0]);
            encodeReg(mCode, asmCon, iex->args[1]);
            encodeReg(mCode, asmCon, iex->args[2]);
            break;
        }
        case singleReg:
        case regPtr: {
            encodeReg(mCode, asmCon, iex->args[0]);
            break;
        }
        case singleLit:
        case singleMem: {
            encodeLitOrMem(mCode, asmCon, iex->args[0]);
            break;
        }
        case noArgs: {
            break;
        }
        case singleLit8: {
            encodeLit8OrMem(mCode, asmCon, iex->args[0]);
            break;
        }
    }
}

/*
TODO consider switching evaluate_node to hex_to_int16, since this should just be
Hex literals
*/
void encodeData(uint8_t* mCode, AssemblyContext* asmCon, DataElement* dex) {
    for (int i = 0; i < dex->val_len; i++) {
        if (dex->size == 8) {
            mCode[asmCon->cAddr++] = (uint8_t)evaluate_node(asmCon, dex->values[i], asmCon->readOutput->symbols, asmCon->readOutput->structures);
        } else {
            uint8_t b2yte = evaluate_node(asmCon, dex->values[i], asmCon->readOutput->symbols, asmCon->readOutput->structures);
            mCode[asmCon->cAddr++] = (uint8_t)(b2yte >> 8);
            mCode[asmCon->cAddr++] = (uint8_t)b2yte;
        }
    }
}

void encodeLitOrMem(uint8_t* mCode, AssemblyContext* asmCon, Syntax* lit) {
    uint16_t b2yte = evaluate_node(asmCon, lit, asmCon->readOutput->symbols, asmCon->readOutput->structures);
    mCode[asmCon->cAddr++] = (uint8_t)(b2yte >> 8);
    mCode[asmCon->cAddr++] = (uint8_t)b2yte;
}

void encodeLit8OrMem(uint8_t* mCode, AssemblyContext* asmCon, Syntax* lit) {
    uint16_t b2yte = evaluate_node(asmCon, lit, asmCon->readOutput->symbols, asmCon->readOutput->structures);
    mCode[asmCon->cAddr++] = (uint8_t)b2yte;
}

void encodeReg(uint8_t* mCode, AssemblyContext* asmCon, Syntax* reg) {
    assert(reg->v_type == STRING)
    mCode[asmCon->cAddr++] = (uint8_t)gm_int(asmCon->registersMap, reg->value.ptr);
}
