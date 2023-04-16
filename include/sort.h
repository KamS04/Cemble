#ifndef SORT_HEADER
#define SORT_HEADER

#include<stdlib.h>
#include<stdbool.h>

void* binsearch(size_t isize, void* arr, void* key, int length, int(*cmp)(void*,void*));
void* linsearch(size_t isize, void* arr, void* key, int length, int(*cmp)(void*,void*));
int bin_search_prim(int* arr, int length, int find);
void bubblesort(size_t isize, void* arr, int length, int(*cmp)(void*, void*));
void quicksort(size_t isize, void* arr, int length, int(*cmp)(void*, void*));

#endif