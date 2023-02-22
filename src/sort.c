#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<stdio.h>

unsigned char _hold_on[50];

void swap(size_t isize, void* arr, int a, int b) {
    void* am = arr + (isize * a);
    void* bm = arr + (isize * b);
    // tmp <- a
    memcpy(&_hold_on, am, isize);
    // a <- b
    memcpy(am, bm, isize);
    // b <- tmp
    memcpy(bm, &_hold_on, isize);
}

// if a < b then cmp < 0
// if a == b then cmp == 0
// if a > b then cmp > 0
int _partition(size_t isize, void* arr, int start, int end, int(*cmp)(void*,void*)) {
    int pI = start;
    void* pV = arr + ( isize * (end - 1) );

    for (int i = start; i < end; i++) {
        if ( cmp((arr + (isize*i)), pV) < 0 ) {
            swap(isize, arr, i, pI);
            pI += 1;
        }
    }
    swap(isize, arr, pI, end-1);
    return pI;
}

void _quicksort(size_t isize, void* arr, int start, int end, int(*cmp)(void*,void*)) {
    if (start >= end) { return; }

    int part =  _partition(isize, arr, start, end, cmp);
    _quicksort(isize, arr, start, part, cmp);
    _quicksort(isize, arr, part+1, end, cmp);
}

// if a < b then cmp < 0
// if a == b then cmp == 0
// if a > b then cmp > 0
void quicksort(size_t isize, void* arr, int length, int(*cmp)(void*, void*)) {
    _quicksort(isize, arr, 0, length, cmp);
}

void* binsearch(size_t isize, void* arr, void* key, int length, int(*cmp)(void*,void*)) {
    int low = 0;
    int high = length-1;

    while (low <= high) {
        int mid = (low+high)/2;
        void* mPtr = arr + ( isize * mid );

        int cmpr = cmp(key, mPtr);

        if (cmpr == 0) {
            return mPtr;
        } else if (cmpr > 0) {
            low = mid + 1;
        } else {
            high = mid -1;
        }
    }

    void* lPtr = arr + (isize * low);
    void* hPtr = arr + (isize * high);

    if (low >= 0 && cmp(key, lPtr) == 0) {
        return lPtr;
    } else if (high >= 0 && cmp(key, hPtr) == 0) {
        return hPtr;
    }

    return NULL;
}

int bin_search_prim(int* arr, int length, int find) {
    int low = 0;
    int high = length-1;

    while (low <= high) {
        int mid = (low + high)/2;
        // printf("lo %d hi %d mi %d am %d f %d\n", low, high, mid, arr[mid], find);

        if (find == arr[mid]) {
            return mid;
        } else if (find > arr[mid]) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }

    if (low >= 0 && arr[low] == find) {
        return low;
    } else if (high >= 0 && arr[high] == find) {
        return high;
    }

    return -1;
}

void* linsearch(size_t isize, void* arr, void* key, int length, int(*cmp)(void*,void*)) {
    for (int i = 0; i < length; i++) {
        void* mE  = arr + (isize * i);
        if (cmp(key, mE) == 0) {
            return mE;
        }
    }
    return NULL;
}
