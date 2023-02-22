#include<Windows.h>
#include<stdio.h>

FILETIME _cached_file_time;
SYSTEMTIME _cached_sys_time;
long long _cached_start_time;

void init_timer() {
    GetSystemTime(&_cached_sys_time);
    SystemTimeToFileTime(&_cached_sys_time, &_cached_file_time);
    _cached_start_time = ((long long)_cached_file_time.dwHighDateTime) << 32 | _cached_file_time.dwLowDateTime;
}

long long get_elapsed_time() {
    GetSystemTime(&_cached_sys_time);
    SystemTimeToFileTime(&_cached_sys_time, &_cached_file_time);
    long long currentTime = ((long long)_cached_file_time.dwHighDateTime) << 32 | _cached_file_time.dwLowDateTime;
    return (currentTime - _cached_start_time) / 10000;
}
