#include "MinCRT.h"
#ifdef WIN32
#include <windows.h>
#endif


typedef struct _heap_header
{
	enum
	{
		HEAP_BLOCK_USED = 1;
		HEAP_BLOCK_FREE = 2;
	}type;

	unsigned block_size = 0;
	struct _heap_header *pre;
	struct _heap_header *next;
}heap_header;


static heap_header * heap_list = 0;

int crt_heap_init()
{
	void *base  = 0;
	unsigned heap_size = 1024 * 1024 * 32;  //32M

#ifdef WIN32
	base = VirtualAlloc(0, heap_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if(base == 0)
		return 0;
#else
	base = (void*)brk(0);
	void *end = (char*)base + heap_size;
	end = (void*)brk(end);
	if( end == 0)
		return 0;
#endif


	heap_header *header = (heap_header*)base;
	header->block_size  = heap_size;
	header->type        = HEAP_BLOCK_FREE;
	header->next        = 0;
	header->pre         = 0;

	heap_list = header;
	return 1;
}