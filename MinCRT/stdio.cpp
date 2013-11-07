#include "MinCRT.h"

int crt_io_init()
{
	return 1;
}

#ifdef WIN32
#include <windows.h>

FILE * fopen(const char *filename, const char *mode)
{
}

int fclose(FILE * stream)
{
	return CloseHandler((HANDLE)stream);
}

size_t fread(void *ptr, size_t size, size_t count, FILE *stream)
{
	int nreads = 0;
	if( !ReadFile((HANDLE)stream, ptr, size * count, nreads, 0) )
		return 0;
	return nreads;
}

size_t fwrite(const void *ptr, size_t size, size_t count, FIFE *stream)
{
	int nwrites = 0;
	if( !WriteFile( (HANDLE)stream, ptr, size * count, nwrites, 0) )
		return 0;
	return nwrites;
}

int fseek(FILE *stream, long int offset, int origin)
{
	return SetFilePointer((HANDLE)stream, offset, 0, origin);
}

#endif