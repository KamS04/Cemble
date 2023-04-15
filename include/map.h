#ifndef MAP_SYSTEM_HEADER
#define MAP_SYSTEM_HEADER

#include<stdlib.h>
#include<stdbool.h>

typedef enum {
    MInt,
    MPtr
} mtype;

typedef struct SimpleEntry {
    char* key;
    union {
        int idat;
        void* pdat;
    } data;
} SimpleEntry;

typedef void*(*sfunc)(size_t, void*, void*, int, int(*)(void*,void*));

typedef struct SimpleMap {
    SimpleEntry* arr;
    int length;
    int clength;
    sfunc sf;
    mtype type;
} SimpleMap;

typedef SimpleMap* Map;

Map simplemap_int(char* keys[], int* datas, int len);
Map simplemap_ptr(char* keys[], void** datas, int len);

SimpleEntry* get_etm(Map map, char* key);
void* gm_ptr(Map map, char* key);
int gm_int(Map map, char* key);
bool has_key_old(Map map, char* key);

Map simplemap_int_incom(int final_len);
Map simplemap_ptr_incom(int final_len);
void add_ito(Map map, char* key, int data);
void add_pto_old(Map map, char* key, void* data);
void finish(Map map);

#endif