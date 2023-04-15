#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include "parselib.h"
#include "assemblelib.h"
#include "debugging.h"

char* strsyntax(result* c, bool nl) {
    assert(c->data_type == SYNTAX_TYPE);
    return syntax_to_string(c->data.ptr, nl);
}
void dsyntax(result* c) {
    if (c->data_type != SYNTAX_TYPE) {
        puts("Trying to deallocate not Syntax with the Syntax deallocer");
        exit(3);
    }
    // TODO deallocate value inside of syntax
    kfree(c->data.ptr);
    kfree(c);
}

char* strssyn(result* c, bool nl) {
    assert(c->data_type == SSYN_PAIR_TYPE);
    char* s;
    convert_ret_item_to_str(c->data.ptr, SSYN_PAIR_TYPE, &s);
    return s;
}
void dssyn(result* c) {
    if (c->data_type != SSYN_PAIR_TYPE) {
        puts("Trying to deallocate not SSYN_PAIR_TYPE with the ssyn deallocer");
        exit(3);
    }
    // TODO deallocate value inside of SSynPair
    kfree(c->data.ptr);
    kfree(c);
}

void snd() {
    dealloc_str_data* dt = get_current_dealloc_data();

    dealloc_str_data* ndt = malloc(sizeof(dealloc_str_data));
    ndt->size = dt->size + 2;
    ndt->data_types = malloc(ndt->size * sizeof(int));
    ndt->deallocers = malloc(ndt->size * sizeof(void*));
    ndt->stringers = malloc(ndt->size * sizeof(void*));

    memcpy(ndt->data_types, dt->data_types, dt->size * sizeof(int));
    memcpy(ndt->deallocers, dt->deallocers, dt->size * sizeof(void*));
    memcpy(ndt->stringers, dt->stringers, dt->size * sizeof(void*));
    
    ndt->data_types[dt->size] = SYNTAX_TYPE;
    ndt->data_types[dt->size + 1] = SSYN_PAIR_TYPE;

    ndt->deallocers[dt->size] = &dsyntax;
    ndt->deallocers[dt->size + 1] = &dssyn;

    ndt->stringers[dt->size] = &strsyntax;
    ndt->stringers[dt->size + 1] = &strssyn;

    set_global_dealloc_data(ndt);
}

int setup() {
    init_core_parsers();
    init_common_ass_parsers();
    init_commstruct_parsers();
    init_hl_parsers();
    snd();
    return 1;
}