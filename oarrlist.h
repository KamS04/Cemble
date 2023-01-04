#ifndef OBJECT_ARRAY_LIST_HEADER
#define OBJECT_ARRAY_LIST_HEADER
#include<stdbool.h>

typedef enum {
    CharAL,
    IntAL,
    PtrAL
} PType;

typedef union {
    int inullv;
    char cnullv;
    void* pnullv;
} PNull;

typedef struct {
    int tsize;
    int csize;
    int stsize;
    void* arr;
    bool snull;
    PNull nullv;
    size_t isize;
    PType type;
} MutArrList;

typedef MutArrList* MAL;

MAL create_array_list(PType type, int start_size, int step_size, bool set_null, PNull null_value);
bool reallocate_to(MAL mut, int nsize);
bool reallocate_more(MAL mut, int size);
bool reallocate_step(MAL mut);
void nullify(MAL mut);
bool shrink_to(MAL mut, int);
bool shrink_to_needed(MAL mut);
bool clear(MAL mut);
bool clear_shrink(MAL mut);

bool append_char_to(MAL mut, char it);
bool append_int_to(MAL mut, int it);
bool append_ptr_to(MAL mut, void* it);

void dealloc_la(MAL mut);

#define GET_4_MACRO(_0, _1, _2, _3, _4, NAME, ...) NAME
#define ALLOCATE_IS(type, start) ALLOCATE_ISSSN(type, start, 10, 0, 0)
#define ALLOCATE_ISS(type, start, step) ALLOCATE_ISSSN(type, start, step, 0, 0)
#define ALLOCATE_ISSS(type, start, step, setnull) ALLOCATE_ISSSN(type, start, step, setnull, 0)
#define ALLOCATE_ISSSN(type, start, step, setnull, nullv) create_array_list(type, start, step, setnull, (PNull)nullv)

#define TSIZE(al) al->tsize
#define SIZEOF(al) al->csize
#define STEPSIZE(al) al->stsize
#define ISIZE(al) al->isize
#define CNULV(al) al->nullv.cnullv
#define INULV(al) al->nullv.inullv
#define PNULV(al) al->nullv.pnullv
#define SNUL(al) al->snull
#define AL(al) al->arr
#define ACL(al) ((char*)AL(al))
#define APL(al) ((void**)AL(al))
#define AIL(al) ((int*)AL(al))

#define GET_ALLOCATE_MACRO(type, ...) GET_4_MACRO(_0, ##__VA_ARGS__, ALLOCATE_ISSSN, ALLOCATE_ISSS, ALLOCATE_ISS, ALLOCATE_IS)(type, __VA_ARGS__)
#define AllocateCharAL(...) GET_ALLOCATE_MACRO(CharAL, __VA_ARGS__)
#define AllocateIntAL(...) GET_ALLOCATE_MACRO(IntAL, __VA_ARGS__)
#define AllocatePtrAL(...) GET_ALLOCATE_MACRO(PtrAL, __VA_ARGS__)

#define CAPPEND(al, it) append_char_to(al, it)
#define IAPPEND(al, it) append_int_to(al, it)
#define PAPPEND(al, it) append_ptr_to(al, it)

#endif