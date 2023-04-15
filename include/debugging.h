#ifndef DEBUGGING_HEADER
#define DEBUGGING_HEADER

#include "assemblelib.h"

#ifdef DEBUG
#define kfree(x)    LOG(printf("KFreeing: %p\n", x)); \
                    free(x)
#else
#define kfree(x) free(x)
#endif

#endif