#include<stdio.h>

#include "map.h"
#include "sort.h"
#include "string.h"

#define TOO_BIG_FOR_LINSEARCH 20

SimpleEntry* _cache;
char* _cached_key;

int _cmp_se_old(void* a, void* b) {
    #define sa ((SimpleEntry*)a)
    #define sb ((SimpleEntry*)b)
    return strcmp( sa->key, sb->key );
}

int _cmp_ke_old(void* a, void* b) {
    #define sb ((SimpleEntry*)b)
    return strcmp( (char*)a, sb->key);
}

Map _create_simplemap(char* keys[], void* datas, int len, mtype type) {
    Map map = malloc(sizeof(SimpleMap));
    map->length = len;
    map->arr = malloc(len * sizeof(SimpleEntry));
    map->type = type;

    switch(type) {
        case MInt:
            for (int i = 0; i < len; i++) {
                map->arr[i].key = keys[i];
                map->arr[i].data.idat = ((int*)datas)[i];
            }
            break;
        case MPtr:
            for (int i = 0; i < len; i++) {
                map->arr[i].key = keys[i];
                map->arr[i].data.pdat = ((void**)datas)[i];
            }
    }

    quicksort(sizeof(SimpleEntry), map->arr, len, &_cmp_se_old);
    
    if (len < TOO_BIG_FOR_LINSEARCH) {
        map->sf = &linsearch;
    } else {
        map->sf = &binsearch;
    }

    map->clength = -1;

    return map;
}

Map simplemap_int(char* keys[], int* datas, int len) {
    return _create_simplemap(keys, datas, len, MInt);
}

Map simple_ptr(char* keys[], void** datas, int len) {
    return _create_simplemap(keys, datas, len, MPtr);
}

Map create_incomplete(int final_len, mtype type) {
    Map map = malloc(sizeof(SimpleMap));
    map->length = final_len;
    map->type = type;
    map->arr = malloc(final_len * sizeof(SimpleEntry));
    map->clength = 0;
    map->sf = &linsearch;
    return map;
}

Map simplemap_int_incom(int final_len) {
    return create_incomplete(final_len, MInt);
}

Map simplemap_ptr_incom(int final_len) {
    return create_incomplete(final_len, MPtr);
}

void add_ito(Map map, char* key, int data) {
    map->arr[map->clength].key = key;
    map->arr[map->clength++].data.idat = data;

    if (map->clength > TOO_BIG_FOR_LINSEARCH) {
        map->sf = &binsearch;
    }
    _cached_key = NULL;
}

void add_pto_old(Map map, char* key, void* data) {
    map->arr[map->clength].key = key;
    map->arr[map->clength++].data.pdat = data;

    if (map->clength > TOO_BIG_FOR_LINSEARCH) {
        map->sf = &binsearch;
    }
    _cached_key = NULL;
}

void finish(Map map) {
    map->clength = -1;
    quicksort(sizeof(SimpleEntry), map->arr, map->length, &_cmp_se_old);
}

SimpleEntry* get_etm(Map map, char* key) {
    if (key == _cached_key) {
        return _cache;
    }

    _cached_key = key;
    int fl = map->length;
    if (map->clength >= 0) {
        fl = map->clength;
    }
    _cache = map->sf(sizeof(SimpleEntry), map->arr, key, fl, &_cmp_ke_old);
    return _cache;
}

bool has_key_old(Map map, char* key) {
    return get_etm(map, key) != NULL;
}

void* gm_ptr(Map map, char* key) {
    SimpleEntry* m = get_etm(map, key);
    if (m == NULL) {
        printf("%s Not valid key\n", key);
        exit(3);
    } else {
        return m->data.pdat;
    }
}

int gm_int(Map map, char* key) {
    SimpleEntry* m = get_etm(map, key);
    if (m == NULL) {
        printf("%s Not valid key\n", key);
        exit(3);
    } else {
        return m->data.idat;
    }
}
