#include<string.h>
#include<stdio.h>
#include<stdint.h>

#include "debugging.h"
#include "reader.h"
#include "assemblelib.h"
#include "oarrlist.h"
#include "encode.h"
#include "fc_config.h"
#include "hashmap.h"
#include "context.h"

#define min(a, b) (a < b) ? a: b

// #region Old Count Run
#if 0
CountOutput* _____oldCountRun____(Syntax** parsedAssembly, int len) {
    int structures = 0; int symbols = 0;
    int lines = 0;

    for (int i = 0; i < len; i++) {
        Syntax* syn = parsedAssembly[i];
        switch (syn->type) {
            case DATA:
                lines++;
                // fall through
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
                // fall through
            default:
                break;
        }
    }

    CountOutput* co = malloc( sizeof(CountOutput) );
    co->structures = structures;
    co->symbols = symbols;
    co->lines = lines;
    return co;
}
#endif
// #endregion

void mf_countRun(Syntax** parsedAssembly, int len, CountOutput* out) {
    out->structures = 0;
    out->symbols = 0;
    out->lines = 0;
    out->exports = 0;
    out->exportStructures = 0;

    for (int i =  0; i < len; i++) {
        switch ( parsedAssembly[i]->type ) {
            case DATA:
                out->lines++;
                out->symbols++;
                assert(parsedAssembly[i]->v_type == DATA_ELEMENT_TYPE);
                if (((DataElement*)parsedAssembly[i]->value.ptr)->is_export) {
                    out->exports++;
                }
                break;
            case LABEL:
                out->symbols++;
                assert(parsedAssembly[i]->v_type == LABEL_ELEMENT_TYPE);
                if (((LabelElement*)parsedAssembly[i]->value.ptr)->is_export) {
                    out->exports++;
                }
                break;
            case CONSTANT:
                out->symbols++;
                assert(parsedAssembly[i]->v_type == CONSTANT_ELEMENT_TYPE);
                if (((ConstantElement*)parsedAssembly[i]->value.ptr)->is_export) {
                    out->exports++;
                }
                break;
            case STRUCTURE:
            case ARGUMENT_STRUCTURE:
                out->structures++;
                assert(parsedAssembly[i]->v_type == STRUCTURE_ELEMENT_TYPE);
                if (((StructureElement*)parsedAssembly[i]->value.ptr)->is_export) {
                    out->exportStructures++;
                }
                break;
            case INSTRUCTION:
                out->lines++;
                break;
            default:
                break;
        }
    }

    return;
}

// #region OldReadRun
#if 0
ReadOutput* ___oldreadRun____(Syntax** parsedAssembly, int len, char* asmC, uint16_t cOffset) {
    CountOutput* co = countRun(parsedAssembly, len);

    MAL nLIdxs = AllocateIntAL(co->lines, 20, 1, IPNULLV);
    HashMap *symbols = hashmap_int(NULL, NULL, 0, co->symbols);
    HashMap *structures = hashmap_ptr(NULL, NULL, 0, co->structures);
    uint16_t cAddr = cOffset;

    char oe;

    for (int i = 0; i < len; i++) {
        Syntax* syn = parsedAssembly[i];

        switch (syn->type) {
            case COMMENT: break;
            case LABEL:
                assert(syn->v_type == STRING);
                if ( has_key(symbols, syn->value.ptr) ) {
                    printf("Label: %s already has a binding to %d\n", (char*)syn->value.ptr, ge_int(symbols, syn->value.ptr, &oe));
                    exit(3);
                }
                add_ito(symbols, syn->value.ptr, cAddr);
                break;
            case CONSTANT: {
                ConstantElement* x = syn->value.ptr;
                if ( has_key(symbols, x->name) ) {
                    printf("Constant: %s already has a binding to %d\n", x->name, ge_int(symbols, x->name, &oe));
                    exit(3);
                }
                bool failed = false;
                int value = evaluate_node_core(NULL, x->value, symbols, structures, &failed);
                if (failed) {
                    printf("Constant: %s, cannot use CLINE i.e. line number in commpiletime only constant.", x->name);
                    exit(3);
                }
                add_ito(symbols, x->name, value);
                break;
            }
            case DATA: {
                DataElement* dex = syn->value.ptr;
                if ( has_key(symbols, dex->name) ) {
                    printf("Data: %s already has a binding to %d\n", dex->name, ge_int(symbols, dex->name, &oe) );
                    exit(3);
                }
                add_ito(symbols, dex->name, cAddr);
                cAddr += (dex->size / 8) * dex->val_len;
                IAPPEND(nLIdxs, cAddr);
                break;
            }
            case ARGUMENT_STRUCTURE:
            case STRUCTURE: {
                StructureElement* sex = syn->value.ptr;
                if ( has_key(structures, sex->name) ) {
                    printf("Structure: %s already has a binding\n", sex->name);
                    exit(3);
                }
                uint16_t mOff;
                if (syn->type == STRUCTURE) {
                    mOff = 0;
                } else {
                    mOff = STACK_FRAME_SIZE;
                }

                HashMap* mMap = hashmap_ptr(NULL, NULL, 0, sex->mem_len);
                for (int i = 0; i < sex->mem_len; i++) {
                    SSynPair* ssp = sex->members[i];
                    StructMember* sm = malloc(sizeof(StructMember));
                    sm->name = ssp->key;
                    sm->offset = mOff;
                    bool failed = false;
                    sm->size = evaluate_node_core(NULL, ssp->val, symbols, structures, &failed);
                    if (failed) {
                        printf("Structure: %s, cannot use CLINE i.e. line number when defining compiletime only structure.", sex->name);
                        exit(3);
                    }
                    mOff += sm->size;
                    add_pto(mMap, ssp->key, sm);
                }
                // finish(mMap);
                StructureComp* sc = malloc(sizeof(StructureComp));
                sc->name = sex->name;
                sc->members = mMap;
                sc->totalSize = mOff;
                add_pto(structures, sex->name, sc);
                break;
            }
            default: {
                assert(syn->type == INSTRUCTION);
                InstructionItem* iit = syn->value.ptr;
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
#endif
// #endregion

char labelError[] = "Label: %s already has a binding to %d\n";
char constantError[] = "Constant: %s already has a binding to %d\n";
char dataError[] = "Data: %s already has a binding to %d\n";
char structureError[] = "Structure: %s already has a binding\n";
char structureClineError[] = "Structure: %s attempts to bind to current line\n";

char oddNodeType[] = "Odd Node type given to fin init loop %d : %s\n";
char initConstantError[] = "File %s: Constant - %s already has a binding to %d\n";
char initConstantOtherError[] = "File %s: Export constant - %s is already bound to %d\n";
char clineConstantError[] = "File %s: Constant - %s cannot be bound to \"CLINE\"\n";

// In mf_readRun create hashmaps and initialize any/all variables we can
// Keep track of the variables that are in the export context
// Keep track of variables we could not parse
// Return for a export initialize run


// Any uninitialized variables must be updated BOTH within the local
// symbols map and the GLOBAL symbols map
int mf_readRun(Syntax **parsedAssembly, int len, ReadOutput *out, char **error) {
    // do a count run
    // setup variables as well as we can

    mf_countRun(parsedAssembly, len, &out->cData);

    // arraylist for line indexes
    MAL nLIdxs = AllocateIntAL(out->cData.lines, 20, 1, IPNULLV);
    // hashmaps for others
    out->symbols = hashmap_int(NULL, NULL, 0, out->cData.symbols);
    out->structures = hashmap_ptr(NULL, NULL, 0, out->cData.structures);
    out->tbinit_symbols = hashmap_ptr(NULL, NULL, 0, 0);
    out->exports = hashmap_int(NULL, NULL, 0, out->cData.exports);
    out->exportStructs = hashmap_ptr(NULL, NULL, 0, out->cData.exportStructures);
    out->cAddr = 0;

    char oe;

    Syntax* syn;
    EvalContext econ;
    econ.conSymbols = out->symbols;
    econ.conStructures = out->structures;
    for (int i = 0; i < len; i++) {
        syn = parsedAssembly[i];

        switch(syn->type) {
            case COMMENT: break;
            case LABEL: {
                assert(syn->v_type == LABEL_ELEMENT_TYPE);
                if (has_key(out->symbols, syn->value.ptr)) {
                    int le = snprintf(NULL, 0, labelError, (char*)syn->value.ptr, ge_int(out->symbols, syn->value.ptr, &oe));
                    *error = malloc(le * sizeof(char));
                    sprintf(*error, labelError, (char*)syn->value.ptr, ge_int(out->symbols, syn->value.ptr, &oe));
                    return -1;
                }
                LabelElement *labEl = syn->value.ptr;
                // fuckery
                //add_int(out->symbols, labEl->name, out->cAddr);
                //if (labEl->is_export) {
                add_pto(out->tbinit_symbols, labEl->name, syn); // will be in this shit if that happened
                //}
                break;
            }
            case CONSTANT: {
                ConstantElement *ce = syn->value.ptr;
                if (has_key(out->symbols, ce->name)) {
                    int le = snprintf(NULL, 0, constantError, ce->name, ge_int(out->symbols, ce->name, &oe));
                    *error = malloc(le * sizeof(char));
                    sprintf(*error, constantError, ce->name, ge_int(out->symbols, ce->name, &oe));
                    return -1;
                }
                uint8_t state;
                uint16_t value = core_node_evaluate(&econ, ce->value, &state);
                if (state == CLINE) {
                    int le = snprintf(NULL, 0, constantError, ce->name, ge_int(out->symbols, ce->name, &oe));
                    *error = malloc(le * sizeof(char));
                    sprintf(*error, constantError, ce->name, ge_int(out->symbols, ce->name, &oe));
                    return -1;
                } else if (state == UDEFS) {
                    add_pto(out->tbinit_symbols, ce->name, ce);
                    // not a failure
                } else {
                    add_int(out->symbols, ce->name, value);
                    if (ce->is_export) { // Hehe Export handling
                        add_int(out->exports, ce->name, value);
                    }
                }
                break;
            }
            case DATA: {
                DataElement *dex = syn->value.ptr;
                if (has_key(out->symbols, dex->name)) {
                    int le = snprintf(NULL, 0, dataError, dex->name, ge_int(out->symbols, dex->name, &oe));
                    *error = malloc(le * sizeof(char));
                    sprintf(*error, dataError, dex->name, ge_int(out->symbols, dex->name, &oe));
                    return -1;
                }
                // add_int(out->symbols, dex->name, out->cAddr);
                out->cAddr += (dex->size / 8) * dex->val_len;
                IAPPEND(nLIdxs, out->cAddr);
                // if (dex->is_export) {
                add_pto(out->tbinit_symbols, dex->name, syn);
                // }
                break;
            }
            case ARGUMENT_STRUCTURE:
            case STRUCTURE: {
                StructureElement *sex = syn->value.ptr;
                if (has_key(out->structures, sex->name)) {
                    int le = snprintf(NULL, 0, structureError, sex->name);
                    *error = malloc(le * sizeof(char));
                    sprintf(*error, structureError, sex->name);
                    return -1;
                }
                uint16_t mOff;
                if (syn->type == STRUCTURE) {
                    mOff = 0;
                } else {
                    mOff = STACK_FRAME_SIZE;
                }


                // ready to init
                bool ready = true;
                for (int i = 0; i < sex->mem_len; i++) {
                    SSynPair* ssp = sex->members[i];
                    uint8_t state;
                    core_node_evaluate(&econ, ssp->val, &state);
                    if (oe == CLINE) {
                        int le = snprintf(NULL, 0, structureClineError, sex->name);
                        *error = malloc(sizeof(char) * le);
                        sprintf(*error, structureClineError, sex->name);
                        return -1;
                    } else if (oe == UDEFS) {
                        ready = false;
                        break;
                    }
                }

                if (ready) {
                    HashMap *mMap = hashmap_ptr(NULL, NULL, 0, sex->mem_len);
                    for (int i = 0; i < sex->mem_len; i++) {
                        SSynPair *ssp = sex->members[i];
                        StructMember *sm = malloc(sizeof(StructMember));
                        sm->name = ssp->key;
                        sm->offset = mOff;
                        uint8_t state;
                        sm->size = core_node_evaluate(&econ, ssp->val, &state);
                        // NO NEED TO CHECK STATE ALREADY CHECKED
                        mOff += sm->size;
                        add_pto(mMap, ssp->key, sm);
                    }
                    StructureComp *sc = malloc(sizeof(StructureComp));
                    sc->name = sex->name;
                    sc->members = mMap;
                    sc->totalSize = mOff;
                    add_pto(out->structures, sex->name, sc);
                    if (sex->is_export) {
                        add_pto(out->exportStructs, sex->name, sc);
                    }
                } else {
                    add_pto(out->tbinit_symbols, sex->name, syn);
                }
                
                break;
            }
            default: {
                assert(syn->type == INSTRUCTION);
                InstructionItem *iit = syn->value.ptr;
                out->cAddr += instruction_size( iit->ins->type );
                IAPPEND(nLIdxs, out->cAddr);
            }
        }
    }

    out->nextLineIndexes = AL(nLIdxs);
    out->num_next_lines = SIZEOF(nLIdxs);
    dealloc_la(nLIdxs);

    return 0;
}

// TODO add file name to all errors
int complete_initialize(SParseRead *parseReads, int len_f, HashMap *exportSymbols, HashMap *exportStructures, char **error, char **efile) {
    bool has_uninit = true; // keeps track of any udefs being declared

    // shift all existing symbols + structures
    for (int i = 0; i < len_f; i++) {
        SParseRead cspRead = parseReads[i];
        HashMap *conExports = cspRead.readOutput.exports;
        HashMap *conStructs = cspRead.readOutput.exportStructs;
        for (int i = 0; i < HASHMAPSIZE(conExports); i++) {
            char* key = HASHMAPKEY(conExports, i);
            int val = HASHMAPVALUEI(conExports, i);
            add_int(exportSymbols, key, val);
        }
        for (int i = 0; i < HASHMAPSIZE(conStructs); i++) {
            char *key = HASHMAPKEY(conStructs, i);
            StructureComp* val = HASHMAPVALUEP(conStructs, i);
            add_pto(exportStructures, key, val);
        }

        // Seems like a good idea
        // kfree(conExports->arr);
        // kfree(conExports);
        // kfree(conStructs->arr);
        // kfree(conStructs);
    }
    
    char oe;
    uint8_t soe;
    int initedinrun; // num changes per run
    EvalContext econ;
    econ.startAddr = 0;
    econ.currAddr = 0;
    econ.codeSize = -1;
    econ.exportStructures = exportStructures;
    econ.exportSymbols = exportSymbols;
    // register names are not used in this
    int left_to_init = 1;
    int loop_run = 1;

    while (left_to_init) {
        has_uninit = false;
        initedinrun = 0;
        left_to_init = 0;

        for (int i = 0; i < len_f; i++) {
            SParseRead cspRead = parseReads[i];
            HashMap *conSymbols = cspRead.readOutput.symbols; // we don't care about the individual exports anymore
            HashMap *conStructs = cspRead.readOutput.structures; // ^
            econ.conStructures = conStructs;
            econ.conSymbols = conSymbols;


            if (HASHMAPSIZE(cspRead.readOutput.tbinit_symbols)) {
                has_uninit = true;

                int i = 0;
                while (i < HASHMAPSIZE(cspRead.readOutput.tbinit_symbols)) { // Switching to while loop
                    //for (int i = 0; i < HASHMAPSIZE(cspRead.readOutput.tbinit_symbols); i++) {
                    char *key = HASHMAPKEY(cspRead.readOutput.tbinit_symbols, i); 
                    Syntax *syn = HASHMAPVALUEP(cspRead.readOutput.tbinit_symbols, i);
                    bool delete = false;
                    switch (syn->type) {
                        case LABEL: {
                            assert(syn->v_type == LABEL_ELEMENT_TYPE);
                            LabelElement* labEl = syn->value.ptr;
                            if (has_key(conSymbols, labEl->name)) {
                                int pBound = ge_int(conSymbols, labEl->name, &oe);
                                int le = snprintf(NULL, 0, labelError, labEl->name, pBound);
                                *error = malloc(le * sizeof(char));
                                sprintf(*error, labelError, labEl->name, pBound);
                                *efile = cspRead.filename;
                                return -1;
                            } else if (labEl->is_export && has_key(exportSymbols, labEl->name)) {
                                int pBound = ge_int(exportSymbols, labEl->name, &oe);
                                int le = snprintf(NULL, 0, labelError, labEl->name, pBound);
                                *error = malloc(le * sizeof(char));
                                sprintf(*error, labelError, labEl->name, pBound);
                                *efile = cspRead.filename;
                                return -1;
                            }
                            // assert(labEl->is_export);
                            delete = true;
                            uint8_t relAddr = ge_int(conSymbols, key, &oe);
                            uint8_t actAddr = relAddr + cspRead.segmentOffset;
                            // TODO check hashmap error
                            add_int(conSymbols, key, actAddr);
                            if (labEl->is_export) {
                                add_int(exportSymbols, key, actAddr);
                            }
                            break;
                        }
                        case CONSTANT: {
                            ConstantElement *ce = syn->value.ptr;
                            // Check local variables
                            // Check global variabels if is_export
                            if (has_key(conSymbols, ce->name)) {
                                // cannot have mutliple defs
                                int le = snprintf(NULL, 0, initConstantError, cspRead.filename, ce->name, ge_int(conSymbols, ce->name, &oe));
                                *error = malloc(le * sizeof(char));
                                sprintf(*error, initConstantError, cspRead.filename, ce->name, ge_int(conSymbols, ce->name, &oe));
                                *efile = cspRead.filename;
                                return -1;
                            } else if (ce->is_export && has_key(exportSymbols, ce->name)) {
                                // cannot have multiple defs
                                int le = snprintf(NULL, 0, initConstantOtherError, cspRead.filename, ce->name, ge_int(exportSymbols, ce->name, &oe));
                                *error = malloc(le * sizeof(char));
                                sprintf(*error, initConstantOtherError, cspRead.filename, ce->name, ge_int(exportSymbols, ce->name, &oe));
                                *efile = cspRead.filename;
                                return -1;
                            }

                            // Pass over exports
                            // Pass over current
                            uint16_t value = core_node_evaluate(&econ, ce->value, &soe);
                            if (soe == CLINE) {
                                int le = snprintf(NULL, 0, clineConstantError, cspRead.filename, ce->name);
                                *error = malloc(le * sizeof(char));
                                sprintf(*error, clineConstantError, cspRead.filename, ce->name);
                                *efile = cspRead.filename;
                                return -1;
                            }
                            else if (soe == UDEFS ) {
                                // continue
                                has_uninit = true;
                            }
                            else { // SUCC
                                add_int(conSymbols, ce->name, (int)value);
                                if (ce->is_export) {
                                    // note not adding to individual stuff cause we don't need to
                                    add_int(exportSymbols, ce->name, (int)value);
                                }
                                initedinrun++;
                                delete = true;
                            }
                            break;
                        }
                        case DATA: { // assume export data item
                            assert(syn->v_type == DATA_ELEMENT_TYPE);
                            DataElement* dex = syn->value.ptr;
                            if (has_key(conSymbols, dex->name)) {
                                int pBound = ge_int(conSymbols, dex->name, &oe);
                                int le = snprintf(NULL, 0, dataError, dex->name, pBound);
                                *error = malloc(le * sizeof(char));
                                sprintf(*error, dataError, dex->name, pBound);
                                *efile = cspRead.filename;
                                return -1;
                            } else if (dex->is_export && has_key(exportSymbols, dex->name)) {
                                int pBound = ge_int(conSymbols, dex->name, &oe);
                                int le = snprintf(NULL, 0, dataError, dex->name, pBound);
                                *error = malloc(le * sizeof(char));
                                sprintf(*error, dataError, dex->name, pBound);
                                *efile = cspRead.filename;
                                return -1;
                            }
                            // assert(dex->is_export);
                            delete = true;
                            uint8_t relAddr = ge_int(conSymbols, key, &oe);
                            uint8_t actAddr = relAddr + cspRead.segmentOffset;
                            // TODO check hashmap error
                            add_int(conSymbols, key, actAddr);
                            if (dex->is_export) {
                                add_int(exportSymbols, key, relAddr + cspRead.segmentOffset);
                            }
                            break;
                        }
                        case ARGUMENT_STRUCTURE:
                        case STRUCTURE: {
                            StructureElement *sex = syn->value.ptr;
                            if (has_key(conStructs, sex->name)) {
                                int le = snprintf(NULL, 0, structureError, sex->name);
                                *error = malloc(le * sizeof(char));
                                sprintf(*error, structureError, sex->name);
                                *efile = cspRead.filename;
                                return -1;
                            } else if (sex->is_export && has_key(exportStructures, sex->name)) {
                                int le = snprintf(NULL, 0, structureError, sex->name);
                                *error = malloc(le * sizeof(char));
                                sprintf(*error, structureError, sex->name);
                                *efile = cspRead.filename;
                                return -1;
                            }
                            uint16_t mOff;
                            if (syn->type == STRUCTURE) {
                                mOff = 0;
                            } else {
                                mOff = STACK_FRAME_SIZE;
                            }

                            // check before mallocing
                            bool ready = true;
                            for (int i = 0; i < sex->mem_len; i++) {
                                SSynPair* ssp = sex->members[i];
                                core_node_evaluate(&econ, ssp->val, &soe);
                                if (oe == CLINE) {
                                    int le = snprintf(NULL, 0, structureClineError, sex->name);
                                    *error = malloc(le * sizeof(char));
                                    sprintf(*error, structureClineError, sex->name);
                                    *efile = cspRead.filename;
                                    return -1;
                                    // how the fuck did we get there
                                } else if (oe == UDEFS) {
                                    ready = false;
                                    has_uninit = true;
                                    break;
                                }
                            }

                            if (ready) {
                                HashMap *mMap = hashmap_ptr(NULL, NULL, 0, sex->mem_len);
                                for (int i = 0; i < sex->mem_len; i++) {
                                    SSynPair *ssp = sex->members[i];
                                    StructMember *sm = malloc(sizeof(StructMember));
                                    sm->name = ssp->key;
                                    sm->offset = mOff;
                                    // already checked errors
                                    sm->size = core_node_evaluate(&econ, ssp->val, &soe);
                                    mOff += sm->size;
                                    add_pto(mMap, ssp->key, sm);
                                }
                                StructureComp *sc = malloc(sizeof(StructureComp));
                                sc->name = sex->name;
                                sc->members = mMap;
                                sc->totalSize = mOff;
                                
                                // add to stuff
                                add_pto(conStructs, sex->name, sc);
                                if (sex->is_export) {
                                    add_pto(exportStructures, sex->name, sc);
                                    initedinrun++;
                                    delete = true;
                                }
                            }
                            break;
                        }
                        default: {
                            assert(syn->v_type == STRING);
                            *efile = cspRead.filename;
                            char *ts = "";
                            if (syn->type < 21) {
                                ts = typeStrs[syn->type];
                            }
                            int le = snprintf(NULL, 0, oddNodeType, syn->type, ts);
                            *error = malloc(le * sizeof(char));
                            sprintf(*error, oddNodeType, syn->type, syn->type, ts);
                            // Note this general method loses error data relating
                            // to what node is causing problems
                            // split out to multiple case OR create a name/tracking string
                            // in order to have a better error
                            return -1;
                        }
                    }


                    if (delete) { // next element will shift to current -> no inc index
                        // TODO check success
                        del_key(cspRead.readOutput.tbinit_symbols, key);
                        initedinrun++;
                    } else { // if not deleted then we have to inc index
                        i++;
                    }
                }

                left_to_init += HASHMAPSIZE(cspRead.readOutput.tbinit_symbols);
            }

        }


        // Check uninit status
        if (initedinrun == 0 && has_uninit) {
            // dependency loop
            *error = "Dependency loop occured: symbols left to initialize, but none were initialized in loop.";
            *efile = "";
            return -1;
        } else if (initedinrun == 0 && left_to_init > 0) {
            // no dependencies but nothing initialized ????
            *error = "Error State: Dependency loop did not occur, but no symbols could be initialized.";
            *efile = "";
            return -1;
        } // else if (left_to_init > 0) {
            // successful run continue
        // }

        loop_run++;
    }

    return 1;
}
