#ifndef LOG_HEADER
#define LOG_HEADER

#ifdef DEBUG
#include "c_config.h"
#define LOG(x)  _Pragma("GCC diagnostic push") \
                _Pragma("GCC diagnostic ignored \"-Wunused-value\"") \
                C_DEBUG_MODE && x; \
                _Pragma("GCC diagnostic pop")
#define assert(x) if (!(x)) { printf("Assertion %s, Failed\n", #x); exit(3); }
#else
#define LOG(x)
#define assert(x)
#endif

#endif