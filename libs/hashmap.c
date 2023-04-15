#include<stdint.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "hashmap.h"
#include "sort.h"

uint32_t hasher(char *key) {
    uint32_t hash = 3323198485ul;
    for (; *key; ++key) {
        hash ^= *key;
        hash *= 0x5bd1e995;
        hash ^= hash >> 15;
    }
    return hash;
}

#define sa ((struct HashEntry*)a)
#define sb ((struct HashEntry*)b)

// compare two HashEntry's
int _cmp_se(void *a, void *b) {
    return (sa->hash == sb->hash && strcmp(sa->key, sb->key) == 0);
}

struct comp_ent {
    uint32_t hash;
    char *key;
};

// compare key+hash to HashEntry
int _cmp_ke(void *a, void *b) {
    #define ce ((struct comp_ent*)a)
    return ( (ce->hash == sb->hash) && (strcmp(ce->key, sb->key) == 0) );
}

#undef sa
#undef sb

HashMap* _create_hashmap(char *keys[], void *datas, int len, int expecting, enum _mdtype type) {
    HashMap *map = malloc(sizeof(HashMap));
    map->len = expecting;
    map->clen = len;
    map->type = type;
    map->arr = malloc(expecting * sizeof(struct HashEntry));
    map->freeze = false;

    for (int i = 0; i < len; i++) {
        map->arr[i].key = keys[i];
        map->arr[i].hash = hasher(keys[i]);
        switch(type) {
            case MInt:
                map->arr[i].data.inv = ((int*)datas)[i];
                break;
            case MPtr:
                map->arr[i].data.ptr = ((void**)datas)[i];
                break;
            case MCh:
                map->arr[i].data.ch = ((char*)datas)[i];
                break;
        }
    }

    quicksort(sizeof(struct HashMap), map->arr, len, _cmp_se);

    return map;
}

HashMap* _create_nt_hashmap(char *keys[], union _mdata *datas, int len, int expecting) {
    HashMap *map = malloc(sizeof(HashMap));
    map->len = expecting;
    map->clen = len;
    map->type = MPtr;
    map->arr = malloc(expecting * sizeof(struct HashEntry));
    map->freeze = false;

    for (int i = 0; i < len; i++) {
        map->arr[i].key = keys[i];
        map->arr[i].hash = hasher(keys[i]);
        map->arr[i].data = datas[i];
    }

    quicksort(sizeof(struct HashMap), map->arr, len, _cmp_se);
    return map;
}

HashMap* hashmap_int(char *keys[], int *datas, int len, int expecting) {
    return _create_hashmap(keys, datas, len, expecting, MInt);
}

HashMap* hashmap_ptr(char *keys[], void **datas, int len, int expecting) {
    return _create_hashmap(keys, datas, len, expecting, MPtr);
}

HashMap* hashmap_ch(char *keys[], char *datas, int len, int expecting) {
    return _create_hashmap(keys, datas, expecting, len, MCh);
}

int _cbin_hmap(HashMap* map, char *key, uint32_t hash) {
    int low = 0;
    int high = map->clen - 1;

    while (low <= high) {
        int mid = (low+high)/2;
        struct HashEntry ent = map->arr[mid];

        if (hash == ent.hash) {
            return mid;
        } else if (hash < ent.hash) {
            high = mid - 1;
        } else {
            low = mid + 1;
        }
    }

    if (map->arr[low].hash == hash) {
        return low;
    }

    return -1;
}

int _cbin_finsert(HashMap *map, char *key, uint32_t hash) {
    int low = 0;
    int high = map->clen - 1;

    while (low <= high) {
        int mid = (low + high) / 2;
        uint32_t med = map->arr[mid].hash;
        
        if (hash == med) {
            low = mid;
            break;
        } else if (hash < med) {
            high = mid-1;
        } else {
            low = mid+1;
        }
    }

    return low;
}

int _csearch_hmap(HashMap *map, char *key, uint32_t hash) {
    int r_bin = _cbin_hmap(map, key, hash);

    if (r_bin == -1) return r_bin;

    // go back to first hash that matches
    for (; r_bin >= 0; r_bin--) {
        if (map->arr[--r_bin].hash != hash) {
            break;
        }
    }
    r_bin++;

    // move to hash with same key
    for (; r_bin < map->clen; r_bin++) {
        if (strcmp(map->arr[r_bin].key, key) == 0) {
            break;
        }
        if (map->arr[r_bin].hash != hash) {
            r_bin = -1;
            break;
        }
    }

    return (r_bin == map->clen) ? -1 : r_bin;
}

int _add_dto(HashMap *map, char *key, union _mdata data) {
    // Insertion find the idx
    uint32_t hash = hasher(key);
    int i = _cbin_finsert(map, key, hash); // insertion index

    // Maybe start chunking???

    if (map->freeze) {
        // Array is finished
        struct HashEntry *temp = NULL;
        size_t iSize = sizeof(struct HashEntry) * map->clen;
        size_t nSize = iSize + sizeof(struct HashEntry);
        // temp = realloc(map->arr, nSize);
        if (temp == NULL) { // Fails often
            // try malloc
            temp = malloc(nSize);
            if (temp == NULL) {
                printf("errno %d, ENOMEM: %d\n", errno, ENOMEM);
                return -1;
            }
            memcpy(temp, map->arr, iSize);
        }
        map->arr = temp;
        map->len++;
    }
    
    if (i < map->clen) {
        memmove(&map->arr[i+1], &map->arr[i], (map->clen - i) * sizeof(struct HashEntry) );
    }

    map->arr[i].key = key;
    map->arr[i].hash = hash;
    map->arr[i].data = data;

    map->clen++;
    map->freeze = map->clen >= map->len;

    return 0;
}

int add_pto(HashMap *map, char *key, void *ptr) {
    return _add_dto(map, key, (union _mdata){ .ptr = ptr });
}

int add_int(HashMap *map, char *key, int inty) {
    return _add_dto(map, key, (union _mdata){ .inv = inty });
}

int add_ch(HashMap *map, char *key, char ch) {
    return _add_dto(map, key, (union _mdata){ .ch = ch });
}

int _ge_idx(HashMap *map, char *key) {
    return _csearch_hmap(map, key, hasher(key));
}

int del_key(HashMap *map, char *key) {
    int idx = _ge_idx(map, key);
    if (idx == -1) {
        return -1; // not found
    }
    memmove(&map->arr[idx], &map->arr[idx+1], (map->clen - idx - 1) * sizeof(struct HashEntry) );
    map->clen--;
    map->freeze = false;
    return 1;
}

struct HashEntry *get_ent(HashMap *map, char *key) {
    int idx = _ge_idx(map, key);
    return (idx == -1) ? NULL : &map->arr[idx];
}

bool has_key(HashMap *map, char *key) {
    return _ge_idx(map, key) != -1;
}
union _mdata _ge_dat(HashMap *map, char *key, char *out) {
    struct HashEntry *m = get_ent(map, key);
    if (m == NULL) {
        *out = 1;
        return (union _mdata){ .ptr = NULL };
    }
    *out = 0;
    return m->data;
}

void *ge_ptr(HashMap *map, char *key, char *out) {
    return _ge_dat(map, key, out).ptr;
}

int ge_int(HashMap *map, char *key, char *out) {
    return _ge_dat(map, key, out).inv;
}

char ge_ch(HashMap *map, char *key, char *out) {
    return _ge_dat(map, key, out).ch;
}

void free_hashmap(HashMap *map) {
    free(map->arr);
    free(map);
}
