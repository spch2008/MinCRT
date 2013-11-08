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


#define va_list char*
#define va_start(ap, arg) (ap=(va_list)&arg+sizeof(arg))
#define va_arg(ap, t) (*(t*)(( ap += sizeof(t)) - sizeof(t)))
#define va_end(ap) (ap=(va_list)0)

int crt_heap_init();
int crt_io_init();
#endif