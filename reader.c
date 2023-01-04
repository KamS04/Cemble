#include<string.h>
#include<stdio.h>
#include<stdint.h>
#include "reader.h"
#include "map.h"
#include "assemblelib.h"
#include "oarrlist.h"
#include "encode.h"
#include "fc_config.h"

CountOutput* countRun(Syntax** parsedAssembly, int len) {
    int structures = 0; int symbols = 0;
    int lines = 0;

    for (int i = 0; i < len; i++) {
        Syntax* syn = parsedAssembly[i];
        switch (syn->type) {
            case DATA:
                lines++;
            case LABEL:
            case CONSTANT:
                symbols++;
                break;
            case STRUCTURE:
            case ARGUMENT_STRUCTURE:
                structures++;
                break;
            case INSTRUCTION:
                lines++;
        }
    }

    CountOutput* co = malloc( sizeof(CountOutput) );
    co->structures = structures;
    co->symbols = symbols;
    co->lines = lines;
    return co;
}

int min(int a, int b) { if ( a < b) return a; return b; }

ReadOutput* readRun(Syntax** parsedAssembly, int len, char* asmC, uint16_t cOffset) {
    CountOutput* co = countRun(parsedAssembly, len);

    MAL nLIdxs = AllocateIntAL(co->lines, 20, 1, 0);
    Map symbols = simplemap_int_incom(co->symbols);
    Map structures = simplemap_ptr_incom(co->structures);
    uint16_t cAddr = cOffset;

    for (int i = 0; i < len; i++) {
        Syntax* syn = parsedAssembly[i];

        switch (syn->type) {
            case COMMENT: break;
            case LABEL:
                assert(syn->v_type == STRING);
                if ( has_key(symbols, syn->value) ) {
                    printf("Label: %s already has a binding to %d\n", (char*)syn->value, gm_int(symbols, syn->value));
                    exit(3);
                }
                add_ito(symbols, syn->value, cAddr);
                break;
            case CONSTANT: {
                ConstantElement* x = syn->value;
                if ( has_key(symbols, x->name) ) {
                    printf("Constant: %s already has a binding to %d\n", x->name, gm_int(symbols, x->name));
                    exit(3);
                }
                int value = evaluate_node(x->value, symbols, structures);
                add_ito(symbols, x->name, value);
                break;
            }
            case DATA: {
                DataElement* dex = syn->value;
                if ( has_key(symbols, dex->name) ) {
                    printf("Data: %s already has a binding to %d\n", dex->name, gm_int(symbols, dex->name) );
                    exit(3);
                }
                add_ito(symbols, dex->name, cAddr);
                cAddr += (dex->size / 8) * dex->val_len;
                IAPPEND(nLIdxs, cAddr);
                break;
            }
            case ARGUMENT_STRUCTURE:
            case STRUCTURE: {
                StructureElement* sex = syn->value;
                if ( has_key(structures, sex->name) ) {
                    printf("Structure: %s already has a binding\n", sex->name);
                    exit(3);
                }
                uint16_t mOff;
                if (syn->type = STRUCTURE) {
                    mOff = 0;
                } else {
                    mOff = STACK_FRAME_SIZE;
                }

                Map mMap = simplemap_ptr_incom(sex->mem_len);
                for (int i = 0; i < sex->mem_len; i++) {
                    SSynPair* ssp = sex->members[i];
                    StructMember* sm = malloc(sizeof(StructMember));
                    sm->name = ssp->key;
                    sm->offset = mOff;
                    sm->size = evaluate_node(ssp->val, symbols, structures);
                    mOff += sm->size;
                    add_pto(mMap, ssp->key, sm);
                }
                finish(mMap);
                StructureComp* sc = malloc(sizeof(StructureComp));
                sc->name = sex->name;
                sc->members = mMap;
                sc->totalSize = mOff;
                add_pto(structures, sex->name, sc);
                break;
            }
            default: {
                assert(syn->type == INSTRUCTION);
                InstructionItem* iit = syn->value;
                cAddr += instruction_size( iit->ins->type );
                IAPPEND(nLIdxs, cAddr);
            }
        }
    }

    finish(symbols);
    finish(structures);
    shrink_to_needed(nLIdxs);

    ReadOutput* ro = malloc(sizeof(ReadOutput));
    ro->symbols = symbols;
    ro->structures = structures;
    ro->nextLineIndexes = AL(nLIdxs);
    ro->num_next_lines = SIZEOF(nLIdxs);
    ro->cAddr = cAddr;

    dealloc_la(nLIdxs);

    return ro;
}
