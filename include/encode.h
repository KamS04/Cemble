#ifndef ENCODE_HEADER
#define ENCODE_HEADER

#include<stdint.h>
#include "assemblelib.h"
#include "encoder.h"
#include "context.h"

uint16_t hex_to_int16(char* hex);
uint16_t eval_arith(uint16_t a, uint16_t b, char op);

uint16_t core_node_evaluate(EvalContext *eval, Syntax *syn, uint8_t *state);

int encoderOfData(uint8_t *mCode, EvalContext *econ, int *mCodeIdx, DataElement *dex);
int encoderOfInstruction(uint8_t *mCode, EvalContext *econ, int *mCodeIdx, InstructionItem *iex);
int encoderOfLitOrMem(uint8_t *mCode, EvalContext *econ, int *mCodeIdx, Syntax *lit, uint8_t *state);
int encoderOfReg(uint8_t *mCode, EvalContext *econ, int *mCodeIdx, Syntax* reg, uint8_t *state);
int encoderOfLit8(uint8_t *mCode, EvalContext *econ, int *mCodeIdx, Syntax *lit, uint8_t *state);

#define UDEFS 1
#define CLINE 2
#define SUCC  3

#endif