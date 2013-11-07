#ifndef _MINCRT_H
#define _MINCRT_H

#define NULL 0

typedef int FILE;
#define EOF (-1)

#ifdef WIN32
#define stdin  ( (FILE*)(GetStdHandle(STD_INPUT_HANDLE)))
#define stdout ( (FILE*)(GetStdHandle(STD_OUTPUT_HANDLE)))
#define stderr ( (FILE*)(GetStdHandle(STD_ERROR_HANDLE)))
#else
#define stdin  ((FILE*)0)
#define stdout ((FILE*)1)
#define stderr ((FILE*)2)
#endif

int crt_heap_init();
int crt_io_init();
#endif