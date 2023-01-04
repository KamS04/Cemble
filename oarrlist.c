#include<stdbool.h>
#include<stdlib.h>
#include<string.h>
#include "oarrlist.h"

MAL create_array_list(PType type, int start_size, int step_size, bool set_null, PNull null_value) {
    MAL mut = malloc(sizeof(MutArrList));
    SIZEOF(mut) = 0;
    TSIZE(mut) = start_size;
    STEPSIZE(mut) = step_size;
    SNUL(mut) = set_null;
    mut->type = type;
    switch(type) {
        case PtrAL: {
            ISIZE(mut) = sizeof(void*);
            AL(mut) = malloc(start_size * sizeof(void*));
            PNULV(mut) = null_value.pnullv;
            break;
        }
        case CharAL: {
            ISIZE(mut) = sizeof(char);
            AL(mut) = malloc(start_size * sizeof(char));
            CNULV(mut) = null_value.cnullv;
            break;
        }
        case IntAL: {
            ISIZE(mut) = sizeof(int);
            AL(mut) = malloc(start_size * sizeof(int));
            INULV(mut) = null_value.inullv;
            break;
        }
    }
    nullify(mut);
    return mut;
}

bool reallocate_to(MAL mut, int nsize) {
    void* na = realloc(AL(mut), nsize * ISIZE(mut));
    if (na == NULL) {
        na = malloc(nsize * ISIZE(mut));
        if (na == NULL) {
            return false;
        }
        memcpy(na, AL(mut), SIZEOF(mut) * ISIZE(mut));
        free(AL(mut));
    }
    AL(mut) = na;
    nullify(mut);
    return true;
}

bool reallocate_more(MAL mut, int size) {
    int needed = TSIZE(mut) + size;
    needed += STEPSIZE(mut) - ( size % STEPSIZE(mut) );
    return reallocate_to(mut, needed);
}

bool reallocate_step(MAL mut) {
    return reallocate_to(mut, TSIZE(mut) + STEPSIZE(mut));
}

void nullify(MAL mut) {
    if (SNUL(mut)) {
        switch(mut->type) {
            case CharAL: {
                for (int i = SIZEOF(mut); i < TSIZE(mut); i++) {
                    ACL(mut)[i] = CNULV(mut);
                }
                break;
            }
            case IntAL: {
                for (int i = SIZEOF(mut); i < TSIZE(mut); i++) {
                    AIL(mut)[i] = INULV(mut);
                }
                break;
            }
            case PtrAL: {
                for (int i = SIZEOF(mut); i < TSIZE(mut); i++) {
                    APL(mut)[i] = PNULV(mut);
                }
                break;
            }
        }
    }
}

bool shrink_to_needed(MAL mut) {
    if (SIZEOF(mut) < TSIZE(mut)) {
        shrink_to(mut, SIZEOF(mut));
    }
    return true;
}

bool shrink_to(MAL mut, int size) {
    TSIZE(mut) = size;
    return reallocate_to(mut, size);
}

bool clear(MAL mut) {
    SIZEOF(mut) = 0;
    nullify(mut);
    return true;
}

bool clear_shrink(MAL mut) {
    SIZEOF(mut) = 0;
    nullify(mut);
    SIZEOF(mut) = STEPSIZE(mut);
    if (!shrink_to_needed(mut)) {
        return false;
    }
    SIZEOF(mut) = 0;
    return true;
}

bool append_char_to(MAL mut, char it) {
    if (SIZEOF(mut) == TSIZE(mut)) {
        if (!reallocate_step(mut)) {
            return false;
        }
    }
    ACL(mut)[SIZEOF(mut)++] = it;
    return true;
}

bool append_int_to(MAL mut, int it) {
    if (SIZEOF(mut) == TSIZE(mut)) {
        if (!reallocate_step(mut)) {
            return false;
        }
    }
    AIL(mut)[SIZEOF(mut)++] = it;
    return true;
}

bool append_ptr_to(MAL mut, void* it) {
    if (SIZEOF(mut) == TSIZE(mut)) {
        if (!reallocate_step(mut)) {
            return false;
        }
    }
    APL(mut)[SIZEOF(mut)++] = it;
    return true;
}

void dealloc_la(MAL mut) {
    free(mut);
}
