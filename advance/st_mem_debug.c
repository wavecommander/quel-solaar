#include <stdlib.h>
#include <stdio.h>
#define ST_NO_MEMORY_DEBUG
#include "st_types.h"

#define ST_MEMORY_OVER_ALLOC 32
#define ST_MEMORY_MAGIC_NUMBER 132
typedef struct{
	uint size;
	void *buf;
}STMemAllocBuf;

typedef struct{
	uint line;
	char file[256];
	STMemAllocBuf *allocs;
	uint alloc_count;
	uint alloc_alocated;
	uint size;
	uint alocated;
	uint freed;
}STMemAllocLine;

STMemAllocLine st_allock_lines[1024];
uint st_allock_line_count = 0;

void st_debug_mem_add(void *pointer, uint size, char *file, uint line)
{
	uint i, j, k;
	for(i = 0; i < ST_MEMORY_OVER_ALLOC; i++)
		((uint8 *)pointer)[size + i] = ST_MEMORY_MAGIC_NUMBER;
	for(i = 0; i < st_allock_line_count; i++)
	{
		for(j = 0; j < st_allock_lines[i].alloc_count; j++)
		{
			uint8 *buf;
			uint size;
			buf = st_allock_lines[i].allocs[j].buf;
			size = st_allock_lines[i].allocs[j].size;
			for(k = 0; k < ST_MEMORY_OVER_ALLOC; k++)
				if(buf[size + k] != ST_MEMORY_MAGIC_NUMBER)
					break;
			if(k < ST_MEMORY_OVER_ALLOC)
				printf("MEM ERROR: Overshoot at line %u in file %s\n", st_allock_lines[i].line, st_allock_lines[i].file);
		}
	}

	for(i = 0; i < st_allock_line_count; i++)
	{
		if(line == st_allock_lines[i].line)
		{
			for(j = 0; file[j] != 0 && file[j] == st_allock_lines[i].file[j] ; j++);
			if(file[j] == st_allock_lines[i].file[j])
				break;
		}
	}
	if(i < st_allock_line_count)
	{
		if(st_allock_lines[i].alloc_alocated == st_allock_lines[i].alloc_count)
		{
			st_allock_lines[i].alloc_alocated += 1024;
			st_allock_lines[i].allocs = realloc(st_allock_lines[i].allocs, (sizeof *st_allock_lines[i].allocs) * st_allock_lines[i].alloc_alocated);
		}
		st_allock_lines[i].allocs[st_allock_lines[i].alloc_count].size = size;
		st_allock_lines[i].allocs[st_allock_lines[i].alloc_count++].buf = pointer;
		st_allock_lines[i].size += size;
		st_allock_lines[i].alocated++;
	}else
	{
		if(i < 1024)
		{
			st_allock_lines[i].line = line;
			for(j = 0; j < 255 && file[j] != 0; j++)
				st_allock_lines[i].file[j] = file[j];
			st_allock_lines[i].file[j] = 0;
			st_allock_lines[i].alloc_alocated = 1024;
			st_allock_lines[i].allocs = malloc((sizeof *st_allock_lines[i].allocs) * st_allock_lines[i].alloc_alocated);
			st_allock_lines[i].allocs[0].size = size;
			st_allock_lines[i].allocs[0].buf = pointer;
			st_allock_lines[i].alloc_count = 1;
			st_allock_lines[i].size = size;
			st_allock_lines[i].freed = 0;
			st_allock_lines[i].alocated++;
			st_allock_line_count++;
		}
	}
}

void *st_debug_mem_malloc(uint size, char *file, uint line)
{
	void *pointer;
	uint i, j, k;
	pointer = malloc(size + ST_MEMORY_OVER_ALLOC);
	st_debug_mem_add(pointer, size, file, line);
	return pointer;
}

void st_debug_mem_remove(void *buf)
{
	uint i, j;
	for(i = 0; i < st_allock_line_count; i++)
	{
		for(j = 0; j < st_allock_lines[i].alloc_count; j++)
		{
			if(st_allock_lines[i].allocs[j].buf == buf)
			{
				st_allock_lines[i].size -= st_allock_lines[i].allocs[j].size;
				st_allock_lines[i].allocs[j] = st_allock_lines[i].allocs[--st_allock_lines[i].alloc_count];
				st_allock_lines[i].freed++;
				return;
			}
		}	
	}
}

void st_debug_mem_free(void *buf)
{
	st_debug_mem_remove(buf);
	free(buf);
}


void *st_debug_mem_realloc(void *pointer, uint size, char *file, uint line)
{
	st_debug_mem_remove(pointer);
	pointer = realloc(pointer, size + ST_MEMORY_OVER_ALLOC);
	st_debug_mem_add(pointer, size, file, line);
	return pointer;
}

void st_debug_mem_print(uint min_allocs)
{
	uint i;
	printf("Memory repport:\n----------------------------------------------\n");
	for(i = 0; i < st_allock_line_count; i++)
	{
		if(min_allocs < st_allock_lines[i].alocated)
		{
			printf("%s line: %u\n",st_allock_lines[i].file, st_allock_lines[i].line);
			printf(" - s: %u\n - a: %u\n - f: %u\n\n", st_allock_lines[i].size, st_allock_lines[i].alocated, st_allock_lines[i].freed);
		}
	}
	printf("----------------------------------------------\n");
}


void st_debug_mem_reset()
{
	uint i;
/*	for(i = 0; i < st_allock_line_count; i++)
		free(st_allock_lines[i].allocs);*/
	st_allock_line_count = 0;
}