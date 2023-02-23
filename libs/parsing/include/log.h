#ifndef LOG_HEADER
#define LOG_HEADER

#ifdef DEBUG
#include "c_config.h"
#define LOG(x)  _Pragma("GCC diagnostic push") \
                _Pragma("GCC diagnostic ignored \"-Wunused-value\"") \
                C_DEBUG_MODE && x; \
                _Pragma("GCC diagnostic pop")

#else
#define LOG(x)
#endif

#endif