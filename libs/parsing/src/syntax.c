#include "syntax.h"
#include "parselib.h"
#include "types.h"
#include "stdlib.h"
#include<stdio.h>
#include<string.h>
#include "ret_types.h"
#include "log.h"

char perfectarray[153];

char* syntax_to_string(Syntax* syn, bool nl) {
    char* format;
    int vidx;
    if (nl) {
        format = "Syntax {\n\ttype: %s,\n\tdtype: %d,\n\tvalue: %s\n}";
        vidx = 41;
    } else {
        format = "Syntax { type: %s, dtype: %d, value: %s }";
        vidx = 38;
    }

    char* tp = typeStrs[syn->type];

    // int l = snprintf(NULL, 0, format, tp, syn->v_type, s) + 1;
    // char* sou = malloc(l * sizeof(char));
    // sprintf(sou, format, tp, syn->v_type, s);
    // snprintf(sou, l, format, tp, syn->v_type, s);

    int l;
    char* sou;

    switch(syn->v_type) {
        case STRING:
            l = snprintf(NULL, 0, format, tp, syn->v_type, syn->value.ptr);
            sou = malloc(l * sizeof(char));
            sprintf(sou, format, tp, syn->v_type, syn->value.ptr);
            break;
        case INTEGER:
            format[vidx] ='d';
            l = snprintf(NULL, 0, format, tp, syn->v_type, syn->value.in);
            sou = malloc(l * sizeof(char));
            sprintf(sou, format, tp, syn->v_type, syn->value.in);
            break;
        case CHAR:
            format[vidx] = 'c';
            l = snprintf(NULL, 0, format, tp, syn->v_type, syn->value.ch);
            sou = malloc(l * sizeof(char));
            sprintf(sou, format, tp, syn->v_type, syn->value.ch);
            break;
        case RES_ARR: {
            ResArrD* _rad = syn->value.ptr;
            char** _vi = malloc( _rad->a_len * sizeof(char*) );
            int f_size = 0;
            
            result* r;
            if (_rad->all_same_type) {
                if (_rad->all_type == SYNTAX_TYPE) {
                    for (int i = 0; i < _rad->a_len; i++) {
                        _vi[i] = syntax_to_string(_rad->arr[i].ptr, false);
                        f_size += strlen(_vi[i]);
                    }
                } else {
                    r = create_result(_rad->all_type, (DataUnion){ .ptr = NULL });
                    for (int i = 0; i < _rad->a_len; i++) {
                        r->data = _rad->arr[i];
                        _vi[i] = dresult_to_string(r, false);
                        f_size += strlen(_vi[i]);
                    }
                }
            } else {
                for (int i = 0; i < _rad->a_len; i++) {
                    r = _rad->arr[i].ptr;
                    if (r->data_type == SYNTAX_TYPE) {
                        _vi[i] = syntax_to_string(r->data.ptr, false);
                    } else {
                        _vi[i] = dresult_to_string(r, false);
                    }
                    f_size += strlen(_vi[i]);
                }
            }
            f_size += 1;
            if (_rad->a_len > 1) {
                f_size += (_rad->a_len - 1) * 2;
            }
            char* s = malloc( f_size *  sizeof(f_size) );

            int c_off = 0;
            for (int i = 0; i < _rad->a_len; i++) {
                int l = strlen(_vi[i]);
                memcpy(s + c_off, _vi[i], l);
                c_off += l;
                if (i < _rad->a_len - 1) {
                    ((char*)s)[c_off++] = ',';
                    ((char*)s)[c_off++] = ' ';
                }
            }
            ((char*)s)[c_off] = '\0';

            l = snprintf(NULL, 0, format, tp, syn->v_type, s);
            sou = malloc(l * sizeof(char));
            sprintf(sou, format, tp, syn->v_type, s);
            free(s);
        }
        case SYNTAX_TYPE: {
            char* s = syntax_to_string((Syntax*)syn->value.ptr, false);
            l = snprintf(NULL, 0, format, tp, syn->v_type, s);
            sou = malloc(l * sizeof(char));
            sprintf(sou, format, tp, syn->v_type, s);
            free(s);
            break;
        }
        default: {
            char* s;
            bool su = convert_ret_item_to_str(syn->value.ptr, syn->v_type, &s);
            if (!su) {
                s = "/UNKNOWN DTYPE/";
            }
            l = snprintf(NULL, 0, format, tp, syn->v_type, s);
            sou = malloc(l * sizeof(char));
            sprintf(sou, format, tp, syn->v_type, s);
            free(s);
        }
    }

    return sou;
}

char* nsyntax_to_string(Syntax* syn) {
    return syntax_to_string(syn, true);
}

Syntax* create_syntax(Types stype, int v_type, DataUnion value) {
    Syntax* v = malloc(sizeof(Syntax));
    v->type = stype;
    v->v_type = v_type;
    v->value = value;
    return v;
}

mapresult* syntax_mapper(result* res, DataUnion data) {
    mapresult* it = malloc(sizeof(mapresult));
    it->dealloc_old = false;

    result* nres = malloc(sizeof(result));
    nres->data_type = SYNTAX_TYPE;

    Syntax* syn = create_syntax((Types) data.in, res->data_type, res->data);

    nres->data.ptr = syn;
    it->res = nres;

    return it;
}
