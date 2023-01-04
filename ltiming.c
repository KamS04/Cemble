#include<unistd.h>
#include<sys/time.h>
#include<time.h>

struct timeval _cached_tv;
long long _start_time;
struct timeval _newtime;
long long _new_time;

void init_timer() {
    gettimeofday(&_cached_tv, NULL);
    _start_time = _cached_tv.tv_sec * 1e6 + _cached_tv.tv_usec;
}

long long get_elapsed_time() {
    gettimeofday(&_newtime, NULL);
    _new_time = _newtime.tv_sec * 1e6 + _newtime.tv_usec;
    return (_new_time - _start_time) / 1000;
}
