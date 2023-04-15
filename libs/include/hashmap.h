#ifndef NMAP_HEADER
#define NMAP_HEADER

#include<stdlib.h>
#include<stdint.h>
#include<stdbool.h>

enum _mdtype {
    MInt,
    MPtr,
    MCh
};

union _mdata {
    char ch;
    int inv;
    void* ptr;
};

struct HashEntry {
    char* key;
    uint32_t hash;
    union _mdata data;
};
// int comparefunc(void* el1, void* el2)
typedef int (*comparefunc)(void*, void*);
// void* searchfunc(size_t el_size, void* array, void* key, int length, *comparefunc)
typedef void* (*searchfunc)(size_t, void*, void*, int, comparefunc);

typedef struct HashMap {
    struct HashEntry* arr;
    int len;
    int clen;
    enum _mdtype type;
    bool freeze;
} HashMap;

#define HASHMAPSIZE(map) map->clen
#define HASHMAPKEY(map, idx) map->arr[idx].key
#define HASHMAPVALUE(map, idx) map->arr[idx].data
#define HASHMAPVALUEI(map, idx) map->arr[idx].data.inv
#define HASHMAPVALUEP(map, idx) map->arr[idx].data.ptr
#define HASHMAPVALUEC(map, idx) map->arr[idx].data.ch

HashMap* hashmap_int(char *keys[], int *datas, int len, int expecting);
HashMap* hashmap_ptr(char *keys[], void **datas, int len, int expecting);
HashMap* hashmap_ch(char *keys[], char *datas, int len, int expecting);

int add_pto(HashMap *map, char *key, void *ptr);
int add_int(HashMap *map, char *key, int inty);
int add_ch(HashMap *map, char *key, char ch);

int del_key(HashMap *map, char *key);

struct HashEntry *get_ent(HashMap *map, char *key);
bool has_key(HashMap *map, char *key);

void *ge_ptr(HashMap *map, char *key, char *out);
int ge_int(HashMap *map, char *key, char *out);
char ge_ch(HashMap *map, char *key, char *out);

void free_hashmap(HashMap *map);

#endif