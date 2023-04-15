#include<stdio.h>
#include "assemblelib.h"
#include "encoder.h"
#include "reader.h"
#include "encode.h"
#include "context.h"

int encoderOfAsm(AssemblingContext asmCon, uint8_t *mCode, HashMap *exportSymbols, HashMap *exportStructures) {
    EvalContext econ = {
        .exportStructures = exportStructures,
        .exportSymbols = exportSymbols,
        .registerNames = asmCon.registersMap
    };

    int mCodeIdx = 0;
    for (int i = 0; i < asmCon.len_f; i++) {
        SParseRead cspRead = asmCon.sparseReads[i];

        econ.startAddr = cspRead.segmentOffset;
        econ.currAddr = 0;
        econ.codeSize = cspRead.readOutput.cAddr;
        econ.conSymbols = cspRead.readOutput.symbols;
        econ.conStructures = cspRead.readOutput.structures;

        for (int j = 0; j < cspRead.lenAst; j++) {
            Syntax* syn = cspRead.ast[j];

            switch (syn->type) {
                case LABEL:
                case CONSTANT:
                case STRUCTURE:
                case ARGUMENT_STRUCTURE:
                case COMMENT:
                    break;

                case DATA: {
                    assert(syn->v_type == DATA_ELEMENT_TYPE);
                    DataElement* dex = syn->value.ptr;
                    encoderOfData(mCode, &econ, &mCodeIdx, dex);
                    break;
                }

                case INSTRUCTION: {
                    assert(syn->v_type == INSTRUCTION_ITEM_TYPE);
                    InstructionItem *iex = syn->value.ptr;
                    encoderOfInstruction(mCode, &econ, &mCodeIdx, iex);
                    break;
                }

                default:
                    break;
            }
        }
    }
    return 1;
}

#if 0
uint8_t* encodeAsm(AssemblyContext* asmCon) {
    uint8_t* mCode = malloc( ( asmCon->readOutput->cAddr - asmCon->sAddr) * sizeof(uint8_t));
    asmCon->cAddr = 0;

    for (int i = 0; i < asmCon->len; i++) {
        Syntax* syn = asmCon->parsedAssembly[i];

        switch (syn->type) {
            case LABEL:
            case CONSTANT:
            case STRUCTURE:
            case ARGUMENT_STRUCTURE:
            case COMMENT:
                break;

            case DATA: {
                assert(syn->v_type == DATA_ELEMENT_TYPE)
                DataElement* dex = syn->value.ptr;
                encodeData(mCode, asmCon, dex);
                break;
            }

            case INSTRUCTION: {
                assert(syn->v_type == INSTRUCTION_ITEM_TYPE)
                InstructionItem* iex = syn->value.ptr;
                encodeInstruction(mCode, asmCon, iex);
                break;
            }

            default:
                break;
        }
    }

    return mCode;
}
#endif
