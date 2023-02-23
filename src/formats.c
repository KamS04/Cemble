#include<stdint.h>
#include<stdio.h>
#include "parselib.h"
#include "assemblelib.h"
#include "instruction.h"
#include "formats.h"

InstructionItem* create_insitem(Instruction* ins, Syntax** args, int args_s) {
    InstructionItem* it = malloc(sizeof(InstructionItem));
    it->ins = ins;
    it->args = args;
    it->args_s = args_s;
    return it;
}

mapresult* singleMapper(result* r, DataUnion data) {
    #define ins ((Instruction*)data.ptr)
    #define rad ((ResArrD*)r->data.ptr)
    #define rarr (rad->arr)
    if (r->data_type != RES_ARR) {
        puts("WTF map did not return an array");
        exit(3);
    }
    mapresult* mr = malloc(sizeof(mapresult));
    mr->dealloc_old = false;
    void** a = malloc(sizeof(void*));
    a[0] = ((result*)rarr[2].ptr)->data.ptr;
    Syntax* syn = create_syntax(
        INSTRUCTION,
        INSTRUCTION_ITEM_TYPE,
        (DataUnion){ .ptr = create_insitem(ins, (Syntax**)a, 1) }
    );
    mr->res = create_result(SYNTAX_TYPE, (DataUnion){ .ptr = syn });
    free(rarr[2].ptr);
    rarr[2].ptr = NULL;
    deallocate_result(r);
    return mr;
    #undef ins
    #undef rad
    #undef rarr
}
parser* createSingle(Instruction* ins, parser* arg) {
    parser** _seqArr = malloc( 4 * sizeof(parser*) );
    _seqArr[0] = upperLowerStrP(ins->mnemonic);
    _seqArr[1] = whitespace;
    _seqArr[2] = arg;
    _seqArr[3] = optionalWhitespace;
    return map(
        sequenceOf(_seqArr, 4),
        singleMapper, false, (DataUnion){ .ptr = ins }
    );
}

mapresult* doubleMapper(result* r, DataUnion data) {
    #define ins ((Instruction*)data.ptr)
    #define rad ((ResArrD*)r->data.ptr)
    #define darr rad->arr
    #define rarr(x) ((result*)(rad->arr[x].ptr))
    if (r->data_type != RES_ARR) {
        puts("WTF map did not return an array");
        exit(3);
    }
    mapresult* mr = malloc(sizeof(mapresult));
    mr->dealloc_old = false;
    Syntax** _pr = malloc(2 * sizeof(Syntax*));
    _pr[0] = rarr(2)->data.ptr;
    _pr[1] = rarr(4)->data.ptr;
    free(darr[2].ptr);
    darr[2].ptr = NULL;
    free(darr[4].ptr);
    darr[4].ptr = NULL;
    Syntax* syn = create_syntax(
        INSTRUCTION,
        INSTRUCTION_ITEM_TYPE,
        (DataUnion){ .ptr = create_insitem(ins, _pr, 2) }
    );
    mr->res = create_result(SYNTAX_TYPE, (DataUnion){ .ptr = syn });
    deallocate_result(r);
    return mr;
    #undef ins
    #undef rad
    #undef darr
    #undef rarr
}
parser* createDouble(Instruction* ins, parser* arg1, parser* arg2) {
    parser** _seqArr = malloc( 6 * sizeof(parser*));
    _seqArr[0] = upperLowerStrP(ins->mnemonic);
    _seqArr[1] = whitespace;
    _seqArr[2] = arg1;
    _seqArr[3] = comma;
    _seqArr[4] = arg2;
    _seqArr[5] = optionalWhitespace;
    return map(
        sequenceOf(_seqArr, 6),
        doubleMapper, false, (DataUnion){ .ptr = ins }
    );
}

mapresult* tripleMapper(result* r, DataUnion data) {
    #define ins ((Instruction*)data.ptr)
    #define rad ((ResArrD*)r->data.ptr)
    #define darr rad->arr
    #define rarr(x) ((result*)(rad->arr[x].ptr))
    if (r->data_type != RES_ARR) {
        puts("WTF map did not return an array");
        exit(3);
    }
    mapresult* mr = malloc(sizeof(mapresult));
    mr->dealloc_old = false;
    Syntax** _pr = malloc(3 * sizeof(Syntax*));
    _pr[0] = rarr(2)->data.ptr;
    _pr[1] = rarr(4)->data.ptr;
    _pr[2] = rarr(6)->data.ptr;
    free(darr[2].ptr);
    darr[2].ptr = NULL;
    free(darr[4].ptr);
    darr[4].ptr = NULL;
    free(darr[6].ptr);
    darr[6].ptr = NULL;
    Syntax* syn = create_syntax(
        INSTRUCTION,
        INSTRUCTION_ITEM_TYPE,
        (DataUnion){ .ptr = create_insitem(ins, _pr, 3) }
    );
    mr->res = create_result(SYNTAX_TYPE, (DataUnion){ .ptr = syn });
    deallocate_result(r);
    return mr;
    #undef ins
    #undef rad
    #undef darr
    #undef rarr
}
parser* createTriple(Instruction* ins, parser* arg1, parser* arg2, parser* arg3) {
    parser** _seqArr = malloc(8 * sizeof(parser*));
    _seqArr[0] = upperLowerStrP(ins->mnemonic);
    _seqArr[1] = whitespace;
    _seqArr[2] = arg1;
    _seqArr[3] = comma;
    _seqArr[4] = arg2;
    _seqArr[5] = comma;
    _seqArr[6] = arg3;
    _seqArr[7] = optionalWhitespace;
    return map(
        sequenceOf(_seqArr, 8),
        tripleMapper, false, (DataUnion){ .ptr = ins }
    );
}

mapresult* noArgsMapper(result* r, DataUnion data) {
    #define ins ((Instruction*)data.ptr)
    mapresult* mr = malloc(sizeof(mapresult));
    mr->dealloc_old = true;
    mr->res = create_result(
        SYNTAX_TYPE,
        (DataUnion){ .ptr = create_syntax(
            INSTRUCTION,
            INSTRUCTION_ITEM_TYPE,
            (DataUnion){ .ptr = create_insitem(ins, NULL, 0) }
        )}
    );
    return mr;
    #undef ins
}
parser* fNoArgs(Instruction* ins) {
    return map(
        then( upperLowerStrP(ins->mnemonic), optionalWhitespace, false),
        noArgsMapper, false, (DataUnion){ .ptr = ins }
    );
}

parser* fSingleLit(Instruction* ins) {
    return createSingle(ins, literal);
}

parser* fSingleReg(Instruction* ins) {
    return createSingle(ins, registers);
}

parser* fSingleMem(Instruction* ins) {
    return createSingle(ins, memory);
}

parser* fLitReg(Instruction* ins) {
    return createDouble(ins, literal, registers);
}

parser* fRegLit(Instruction* ins) {
    return createDouble(ins, registers, literal);
}

parser* fRegReg(Instruction* ins) {
    return createDouble(ins, registers, registers);
}

parser* fRegMem(Instruction* ins) {
    return createDouble(ins, registers, memory);
}

parser* fMemReg(Instruction* ins) {
    return createDouble(ins, memory, registers);
}

parser* fLitMem(Instruction* ins) {
    return createDouble(ins, literal, memory);
}

parser* fRegPtrReg(Instruction* ins) {
    return createDouble(ins, registerPtr, registers);
}

parser* fRegRegPtr(Instruction* ins) {
    return createDouble(ins, registers, registerPtr);
}

parser* fRegPtr(Instruction* ins) {
    return createSingle(ins, registerPtr);
}

parser* fLitOffReg(Instruction* ins) {
    return createTriple(ins, literal, registerPtr, registers);
}

insParserCreator typeCreatorMap[] = {
    fLitReg,
    fRegLit,
    fRegLit,
    fRegReg,
    fRegMem,
    fMemReg,
    fLitMem,
    fLitMem,
    fRegPtrReg,
    fLitOffReg,
    fNoArgs,
    fSingleReg,
    fSingleLit,
    fRegRegPtr,
    fSingleMem,
    fRegPtr,
    fSingleLit
};
