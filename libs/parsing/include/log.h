#ifndef LOG_HEADER
#define LOG_HEADER

#ifdef DEBUG
#include "c_config.h"
#define LOG(x) C_DEBUG_MODE && x
#else
#define LOG(x)
#endif

#endif