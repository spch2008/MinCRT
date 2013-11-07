#include "MinCRT.h"
#ifdef WIN32
#include <windows.h>
#endif


typedef struct _heap_header
{
	enum
	{
		HEAP_BLOCK_USED = 1,
		HEAP_BLOCK_FREE = 2,
	}type;

	unsigned size;
	struct _heap_header *pre;
	struct _heap_header *next;
}heap_header;

#define HEADER_SIZE sizeof(heap_header)
#define ADDR_ADD(base, incre) ((char*)(base) + (incre))

static heap_header * heap_list = 0;

#ifndef WIN32
static int brk(void *end_data_segment)
{
	int ret = 0;
	
	asm( "movl $45, %%eax \n\t"
	     "movl %1,  %%ebx \n\t"
		 "int $0x80       \n\t"
		 : "=r"(ret) : "m"(end_data_segment));

	return ret;
}
#endif

static int crt_heap_init()
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
	header->size  = heap_size;
	header->type        = heap_header::HEAP_BLOCK_FREE;
	header->next        = 0;
	header->pre         = 0;

	heap_list = header;
	return 1;
}

void *malloc(unsigned size)
{
	if(size == 0)
		size = 1;

	heap_header *curr = heap_list;
	while( curr != 0)
	{
		if(curr->type == heap_header::HEAP_BLOCK_USED)
		{
			curr = curr -> next;
			continue;
		}

		//大于正好或多一点点，没办法切分
		if(curr->size > size + HEADER_SIZE 
			&& curr->size <= size + HEADER_SIZE * 2)
		{
			curr->type = heap_header::HEAP_BLOCK_USED;
			return ADDR_ADD(curr, HEADER_SIZE);
		}

		//区间较大，进行拆分
		if(curr->size > size + HEADER_SIZE * 2)
		{
			heap_header *next =(heap_header*)( (char*)curr + size + HEADER_SIZE );
			next->type = heap_header::HEAP_BLOCK_FREE;
			next->size = curr->size - (size + HEADER_SIZE);
			next->pre  = curr;
			next->next = curr->next;

			curr->type = heap_header::HEAP_BLOCK_USED;
			curr->size = size + HEADER_SIZE;
			curr->next = next;
			return ADDR_ADD(curr, HEADER_SIZE);
		}
		curr = curr->next;
	}
	return 0;
}

void free(void *p)
{
	if( p == 0)
		return;
	heap_header *header = (heap_header*)((char *)p - HEADER_SIZE);
	if(header->type == heap_header::HEAP_BLOCK_FREE)
		return;

	header->type = heap_header::HEAP_BLOCK_FREE;
	
	//与前面块合并
	if(header->pre != 0 && header->pre->type == heap_header::HEAP_BLOCK_FREE)
	{
		header->pre->size += header->size;
		if(header->next != 0)
			header->next->pre = header->pre;

		header->pre->next = header->next;
	}

	//与后面合并
	if(header->next != 0 && header->next->type == heap_header::HEAP_BLOCK_FREE)
	{
		header->size += header->next->size;
		
		if(header-> next ->next != 0)
			header->next -> next -> pre = header;
		header->next = header->next->next;
	}
}

/* 注意：
   1. 采用首次适应算法，数据结构采用链表
   2. 空闲与占用的块统一放在一起，并不单独拿出来，
      一个整体，空闲的前面可能是占用块，也可能是空闲块
   3. 缺点：当一个链指针遭到破坏后，整个堆失效
   4. 信息存在头部，所以头部不能被破坏，且头部占用多余空间存储信息
*/

