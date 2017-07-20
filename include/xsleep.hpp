#ifndef KOSSHI_SLEEP_INCLUDED
#define KOSSHI_SLEEP_INCLUDED 1

// #ifdef LINUX
// #include <unistd.h>
// #endif
// #ifdef WINDOWS
// #include <windows.h>
// #endif

// void xsleep(int sleepMs)
// {
// #ifdef LINUX
//     usleep(sleepMs * 1000);   // usleep takes sleep time in us (1 millionth of a second)
// #endif
// #ifdef WINDOWS
//     Sleep(sleepMs);
// #endif
// }
#include <windows.h>

void xsleep(int ms){
	Sleep(ms);
}

#endif