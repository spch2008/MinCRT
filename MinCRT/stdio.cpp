#include "MinCRT.h"

int crt_io_init()
{
	return 1;
}

#ifdef WIN32
#include <windows.h>

FILE * fopen(const char *filename, const char *mode)
{	
	HANDLE hFile = CreateFileA(filename, GENERIC_WRITE | GENERIC_READ, 0, 0, 1, 0, 0);
	if(hFile == INVALID_HANDLE_VALUE)
		return 0;
	return (FILE*)hFile;
}

int fclose(FILE * stream)
{
	return CloseHandle((HANDLE)stream);
}

size_t fread(void *ptr, size_t size, size_t count, FILE *stream)
{
	int nreads = 0;
	if( !ReadFile((HANDLE)stream, ptr, size * count, (LPDWORD)&nreads, 0) )
		return 0;
	return nreads;
}

size_t fwrite(const void *ptr, size_t size, size_t count, FILE *stream)
{
	int nwrites = 0;
	if( !WriteFile( (HANDLE)stream, ptr, size * count, (LPDWORD)&nwrites, 0) )
		return 0;
	return nwrites;
}

int fseek(FILE *stream, long int offset, int origin)
{
	return SetFilePointer((HANDLE)stream, offset, 0, origin);
}

#else

static int open(const char *pathname, int flags, int mode)
{
	int fd = 0;
	asm(" movl $5, %%eax \n\t"
		" movl %1, %%ebx \n\t"
		" movl %2, %%ecx \n\t"
		" movl %3, %%edx \n\t"
		" int $0x80      \n\t"
		" movl %%eax, %0 \n\t":
		" =m"(fd) : "m"(pathname), "m"(flags), "m"(mode));
	return fd;
}

static int read(int fd, void *buffer, unsigned size)
{
	int ret = 0;
	asm(" movl $3, %%eax \n\t"
		" movl %1, %%ebx \n\t"
		" movl %2, %%ecx \n\t"
		" movl %3, %%edx \n\t"
		" int $0x80      \n\t"
		" movl %%eax, %0 \n\t":
		"=m"(ret) : "m"(fd), "m"(buffer), "m"(size));
	return ret;
}

static int write(int fd, const void *buffer, unsigned size)
{
	int ret = 0;
	asm(" movl $4, %%eax \n\t"
		" movl %1, %%ebx \n\t"
		" movl %2, %%ecx \n\t"
		" movl %3, %%edx \n\t"
		" int $0x80      \n\t"
		" movl %%eax, %0 \n\t":
	    "=m"(ret):"m"(fd), "m"(buffer), "m"(size) );
	return ret;
}


static int close(int fd)
{
	int ret = 0;
	asm(" movl $6, %%eax \n\t"
		" movl %1, %%ebx \n\t"
		" int $0x80      \n\t"
		"movl %%eax, %0  \n\t" :
	    "=m"(ret) : "m"(fd));
	return ret;
}


static int seek(int fd, int offset, int mod)
{
	int ret = 0;
	ams(" movl $19, %%eax \n\t"
		" movl %1,  %%ebx \n\t"
		" movl %2,  %%ecx \n\t"
		" movl %3,  %%edx \n\t"
		" int $0x80       \n\t"
		" movl %%eax, %0  \n\t" :
	    "=m"(ret):"m"(fd),"m"(offset), "m"(mode));
	return ret;
}


FILE * fopen(const char *filename, const char *mode)
{
	int fd = open(filename, 02 | 0100, 0700);
	return (FILE*)fd;
}

int fclose(FILE * stream)
{
	return close(stream);
}

size_t fread(void *ptr, size_t size, size_t count, FILE *stream)
{
	return read(stream, ptr, size * count);
}

size_t fwrite(const void *ptr, size_t size, size_t count, FIFE *stream)
{
	return write(stream, ptr, size * count);
}

int fseek(FILE *stream, long int offset, int origin)
{
	return seek(stream, offset, origin);
}
#endif


int fputc(int character, FILE *stream)
{
	if( fwrite(&character, sizeof(char), 1, stream) == 0)
		return EOF;
	else
		return character;
}


int fputs(const char *str, FILE *stream)
{
	int len = strlen(str);
	if( fwrite(str, sizeof(char), len, stream) == 0)
		return EOF;
	else
		return len;
}

char *strchr(char * str, int character )
{
	while(str != '\0')
	{
		if( *str == character)
			return str;
		str++;
	}
	return NULL;
}

int vfprintf( FILE * stream, const char * format, va_list arg )
{
	const char *f = format;
	int ret = 0;
	while( *f != '\0')
	{
		if(*f != '%')
		{
			const char *next = strchr(f+1, '%');
			if(next == NULL)
				next = strchr(f+1, '\0');
			while(f < next)
			{
				fputc(*f++, stream);
				ret++;
			}
			continue;
		}

		++f;
		if( *f == '%')
		{
			fputc(*f++, stream);
			ret++;
			continue;
		}

		char c = *f;
		switch (c)
		{
		case 'd':
			char buffer[12];
			itoa(va_arg(arg, int), buffer, 10);
			fputs(buffer, stream);
			ret += strlen(buffer);
			break;
		case 's':
			const char *s = va_arg(arg, char *);
			fputs(s, stream);
			ret += strlen(s);
			break;
		}
		++f;
	}
	return ret;
}

int printf ( const char * format, ... )
{
	va_list(list);
	va_start(list, format);
	return vfprintf(stdout, format, list);
}

int fprintf ( FILE * stream, const char * format, ... )
{
	va_list(list);
	va_start(list, format);
	return vfprintf(stream, format, list);
}