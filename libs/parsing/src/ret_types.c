#include "ret_types.h"
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include "syntax.h"
#include "instruction.h"

bool convert_ret_item_to_str(void* item, int type, char** out) {
    switch (type) {
        case SYMBOLIC_ELEMENT_TYPE: {
            SymbolicElement* x = item;
            *out = malloc( (26 + strlen(x->name) ) * sizeof(char) );
            sprintf(*out, "SymbolicElement { name: %s }", x->name);
            break;
        }
        case SSYN_PAIR_TYPE: {
            SSynPair* x = item;
            char* sy = syntax_to_string(x->val, false);
            char* format = "{ %s -> %s }";
            int si = snprintf(NULL, 0, format, x->key, sy) + 1;
            *out = malloc( si * sizeof(char));
            sprintf(*out, format, x->key, sy);
            free(sy);
            break;
        }
        case BINARY_OPERATION_TYPE: {
            BinaryOperation* x = item;
            char* format = "{ %s %c %s }";
            char* a; char* b;
            if (x->a->v_type == BINARY_OPERATION_TYPE) {
                convert_ret_item_to_str(x->a->value.ptr, BINARY_OPERATION_TYPE, &a);
            } else {
                a = syntax_to_string(x->a, false);
            }
            if (x->b->v_type == BINARY_OPERATION_TYPE) {
                convert_ret_item_to_str(x->b->value.ptr, BINARY_OPERATION_TYPE, &b);
            } else {
                b = syntax_to_string(x->b, false);
            }
            int l = snprintf(NULL, 0, format, a, x->operation, b);
            *out = malloc( l * sizeof(char) );
            sprintf(*out, format, a, x->operation, b);
            free(a);
            free(b);
            break;
        }
        case STRUCTURE_ELEMENT_TYPE: {
            StructureElement* x = item;
            char* format;
            int fstr_size = 0;
            char** strs = malloc(x->mem_len * sizeof(char*));
            for (int i = 0; i < x->mem_len; i++) {
                convert_ret_item_to_str(x->members[i], SSYN_PAIR_TYPE, &strs[i]);
                fstr_size += strlen(strs[i]);
                fstr_size += 2;
            }
            fstr_size -= 1;
            char* v_ = malloc(fstr_size * sizeof(char));
            fstr_size = 0;
            for (int i = 0; i < x->mem_len; i++) {
                strcpy(v_ + fstr_size, strs[i]);
                fstr_size += strlen(strs[i]);
                if (i + 1 < x->mem_len) {
                    v_[fstr_size++] = ',';
                    v_[fstr_size++] = ' ';
                }
                free(strs[i]);
            }
            v_[fstr_size] = '\0';
            free(strs);
            if (x->is_export) {
                format = "Structure+ { name: %s, values: [ %s ] }";
            } else {
                format = "Structure { name: %s, values: [ %s ] }";
            }
            int o_size = snprintf(NULL, 0, format, x->name, v_) + 1;
            *out = malloc(o_size * sizeof(char));
            sprintf(*out, format, x->name, v_);
            free(v_);
            break;
        }
        case CONSTANT_ELEMENT_TYPE: {
            ConstantElement* x = item;
            char* s = syntax_to_string(x->value, false);
            char* format;
            *out = malloc( (36 + x->is_export + strlen(x->name) + strlen(s)) * sizeof(char));
            if (x->is_export) {
                format = "ConstantElement+ { name: %s , value: %s }";
            } else {
                format = "ConstantElement { name: %s , value: %s }";
            }
            sprintf(*out, format, x->name, s);
            break;
        }
        case DATA_ELEMENT_TYPE: {
            DataElement* x = item;
            char* format;
            int fstr_size = 0;
            for (int i = 0; i < x->val_len; i++) {
                fstr_size += strlen((char*) x->values[i]->value.ptr);
                fstr_size += 2;
            }
            fstr_size -= 1;
            char* v_ = malloc(fstr_size * sizeof(char));
            fstr_size = 0;
            for (int i = 0; i < x->val_len; i++) {
                strcpy(&v_[fstr_size], x->values[i]->value.ptr);
                fstr_size += strlen(x->values[i]->value.ptr);
                if (i + 1 < x->val_len) {
                    v_[fstr_size++] = ',';
                    v_[fstr_size++] = ' ';
                }
            }
            v_[fstr_size] = '\0';
            if (x->is_export) {
                format = "DataElement+ { name: %s, isize: %d, values: [ %s ] }";
            } else {
                format = "DataElement { name: %s, isize: %d, values: [ %s ] }";
            }
            int o_size = snprintf(NULL, 0, format, x->name, x->size, v_) + 1;
            *out = malloc(o_size * sizeof(char));
            sprintf(*out, format, x->name, x->size, v_);
            free(v_);
            break;
        }
        case INTERPRET_SINGLE_TYPE: {
            InterpretSingle* x = item;
            char* s = syntax_to_string(x->symbol, false);
            char format[] = "InterpretSingle { structure: %s, property: %s, symbol: %s }";
            int o_size = snprintf(NULL, 0, format, x->structure, x->property, s) + 1;
            *out = malloc(o_size * sizeof(char));
            sprintf(*out, format, x->structure, x->property, s);
            free(s);
            break;
        }
        case INTERPRET_ARRAY_TYPE: {
            InterpretArray* x = item;
            char* s = syntax_to_string(x->symbol, false);
            char* i = syntax_to_string(x->index, false);
            char format[] = "InterpretArray { structure: %s, property: %s, symbol: %s, index: %s }";
            int o_size = snprintf(NULL, 0, format, x->structure, x->property, s, i);
            *out = malloc(o_size * sizeof(char));
            sprintf(*out, format, x->structure, x->property, s, i);
            free(s);
            free(i);
            break;
        }
        case INSTRUCTION_ITEM_TYPE: {
            InstructionItem* x = item;
            Instruction* ins = x->ins;
            char* argsStr = "";
            char* a1; char* a2; char* a3;
            int l;
            if (x->args_s == 1) {
                a1 = syntax_to_string(x->args[0], false);
                l = snprintf(NULL, 0, "\t\targ: %s\n", a1) + 1;
                argsStr = malloc(l * sizeof(char));
                sprintf(argsStr, "\t\targ: %s\n", a1);
                free(a1);
            }
            else if (x->args_s == 2) {
                a1 = syntax_to_string(x->args[0], false);
                a2 = syntax_to_string(x->args[1], false);
                l = snprintf(NULL, 0, "\t\targ1: %s\n\t\targ2: %s\n", a1, a2) + 1;
                argsStr = malloc(l * sizeof(char));
                sprintf(argsStr, "\t\targ1: %s\n\t\targ2: %s\n", a1, a2);
                free(a1);
                free(a2);
            }
            else if (x->args_s == 3) {
                a1 = syntax_to_string(x->args[0], false);
                a2 = syntax_to_string(x->args[1], false);
                a3 = syntax_to_string(x->args[2], false);
                l = snprintf(NULL, 0, "\t\targ1: %s\n\t\targ2: %s\n\t\targ3: %s\n", a1, a2, a3) + 1;
                argsStr = malloc(l * sizeof(char));
                sprintf(argsStr, "\targ1: %s\n\t\targ2: %s\n\t\targ3: %s\t\n", a1, a2, a3);
                free(a1);
                free(a2);
                free(a3);
            }

            l = snprintf(NULL, 0, "InstructionItem {\n\t\topcode: %d\ttag: %s\n%s\t}", ins->opcode, ins->tag, argsStr) + 1;
            *out = malloc(l * sizeof(char));
            sprintf(*out, "InstructionItem {\n\t\topcode: %d\ttag: %s\n%s\t}", ins->opcode, ins->tag, argsStr);
            break;
        }
        default:
            return false;
    }

    return true;
}
