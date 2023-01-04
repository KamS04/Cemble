#include<stdio.h>
#include "assemblelib.h"
#include "encoder.h"
#include "reader.h"
#include "encode.h"

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
                DataElement* dex = syn->value;
                encodeData(mCode, asmCon, dex);
                break;
            }

            case INSTRUCTION: {
                assert(syn->v_type == INSTRUCTION_ITEM_TYPE)
                InstructionItem* iex = syn->value;
                encodeInstruction(mCode, asmCon, iex);
                break;
            }
        }
    }

    return mCode;
}
