#include <stdio.h>
#include <stdlib.h>
#include "st_types.h"

void *myalloc(uint size)
{
//	printf("allocating %u bytes\n", size);
/*	if(size == 268435456)
	{
		int *a = 0;
		a[0] = 0;
	}*/
	return malloc(size);
}



void init_dlut(DynLookUpTable *table)
{
	void **data, **end;
	table->data = malloc((sizeof *table->data) * ARRAY_CHUNK_SIZE);
	table->allocated = ARRAY_CHUNK_SIZE;
	data = table->data;
	for(end = data + ARRAY_CHUNK_SIZE; data < end; data++)
		*data = NULL;
}

void free_dlut(DynLookUpTable *table)
{
	free(table->data);	
}

void *find_dlut(DynLookUpTable *table, uint key)
{
	if(key < table->allocated)
		return table->data[key];
	return NULL;
}

void *get_next_dlut(DynLookUpTable *table, uint key)
{
	uint i;
	for(i = key; i < table->allocated && table->data[i] == NULL; i++);
	if(i >= table->allocated)
		return NULL;
	return table->data[i];
}

uint get_next_empty_dlut(DynLookUpTable *table, uint key)
{
	uint i;
	for(i = key; i < table->allocated && table->data[i] != NULL; i++);
	return i;
}

void *add_entry_dlut(DynLookUpTable *table, uint key, void *pointer)
{
	void **data, **end;
	if(key < table->allocated)
	{
		data = table->data[key];
		table->data[key] = pointer;
		return data;
	}
	table->data = realloc(table->data,(sizeof *table->data) * (key + ARRAY_CHUNK_SIZE));
	data = table->data + table->allocated;
	table->allocated = key + ARRAY_CHUNK_SIZE;
	for(end = table->data + table->allocated; data < end; data++)
		*data = NULL;
	table->data[key] = pointer;
	return NULL;
}

uint add_entry_in_empty_dlut(DynLookUpTable *table, void *pointer)
{
	uint id;
	id = get_next_empty_dlut(table, 0);
	add_entry_dlut(table, id, pointer);
	return id;
}

void remove_entry_dlut(DynLookUpTable *table, uint key)
{
	if(key < table->allocated)
	{
		table->data[key] = NULL;
	}
}

uint count_entry_dlut(DynLookUpTable *table)
{
	uint i, count = 0; 
	for(i = 0; i < table->allocated; i++)
		if(table->data[i] != NULL)
			count++;
	return count;
}

void clean_dlut(DynLookUpTable *table)
{
	int i;
	for(i = table->allocated - 1; i > 1 && table->data[i] == NULL; i--);
	if(i + ARRAY_CHUNK_SIZE < table->allocated)
	table->data = realloc(table->data,(sizeof *table->data) * (i + ARRAY_CHUNK_SIZE));
}

void foreach_remove_dlut(DynLookUpTable *table, void (*func)(uint key, void *pointer, void *user_data), void *user_data)
{
	uint i;
	void **data;
	data = table->data;
	for(i = 0; i < table->allocated; i++)
		if(data[i] != NULL)
			func(i, data[i], user_data);
	free(table->data);	
}

double get_rand(uint32 index)
{
	index = (index << 13) ^ index;
	return (((index * (index * index * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0) * 0.5;
}

uint get_randi(uint32 index)
{
	index = (index << 13) ^ index;
	return ((index * (index * index * 15731 + 789221) + 1376312589) & 0x7fffffff);
}

void st_hsv(float *output, float r, float g, float b)
{
	if(r < g)
	{
		if(r < b)
		{
			if(g < b)
			{
				output[0] = (4 - (g - r) / (b - r)) / 6.0;
				output[1] = r / b;
				output[2] = b;
			}else
			{
				output[0] = (2 + (b - r) / (g - r)) / 6.0;
				output[1] = r / g;
				output[2] = g;
			}
		}else
		{
			output[0] = (2 - (r - b) / (g - b)) / 6.0;
			output[1] = b / g;
			output[2] = g;
		}
	}else
	{
		if(r < b)
		{
			output[0] = (4 + (r - g) / (b - g)) / 6.0;
			output[1] = g / b;
			output[2] = b;
		}else
		{
			if(g < b)
			{
				output[0] = (6 - (b - g) / (r - g)) / 6.0;
				output[1] = g / r;
				output[2] = r;
			}else if(r == g && r == b)
			{
				output[0] = 0;
				output[1] = 1;
				output[2] = r;
			}else
			{

				output[0] = (0 + (g - b) / (r - b)) / 6.0;
				output[1] = b / r;
				output[2] = r;
			}
		}
	}
}

void st_rgb(float *output, float h, float s, float v)
{
	if(h > 1 || h < 0)
		h = h - (float)((int)h);
	if(s < 0)
		s = 0;
	if(s > 1)
		s = 1;
	if(v < 0)
		v = 0;
	if(v > 1)
		v = 1;

	h *= 6;
	s *= v;
	switch((uint)h)
	{
		case 0 :
			output[0] = v;
			output[1] = s + (0 + h) * (v - s);
			output[2] = s;
		break;
		case 1 :
			output[0] = s + (2 - h) * (v - s);
			output[1] = v;
			output[2] = s;
		break;
		case 2 :
			output[0] = s;
			output[1] = v;
			output[2] = s + (h - 2) * (v - s);
		break;
		case 3 :
			output[0] = s;
			output[1] = s + (4 - h) * (v - s);
			output[2] = v;
		break;
		case 4 :
			output[0] = s + (h - 4) * (v - s);
			output[1] = s;
			output[2] = v;
		break;
		default :
			output[0] = v;
			output[1] = s;
			output[2] = s + (6 - h) * (v - s);
		break;
	}
}
