
#include "st_types.h"
#include <stdio.h>


#if defined _WIN32
#include <winsock.h>
typedef unsigned int uint;
typedef SOCKET VSocket;
#else
typedef int VSocket;
#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#endif
#include <stdio.h>
#include <stdlib.h>


typedef unsigned int uint32;
typedef int int32;
typedef unsigned short uint16;
typedef short int16;
typedef unsigned char uint8;
typedef char int8;

double me_delta_time = 0.0001;
uint32 me_seconds = 0;
uint32 me_fractions = 0;
float me_sun_vector[4] = {1, 0, 0, 1};
#if defined _WIN32

void a_get_current_time(uint32 *seconds, uint32 *fractions)
{
	static LARGE_INTEGER frequency;
	static boolean init = FALSE;
	LARGE_INTEGER counter;

	if(!init)
	{
		init = TRUE;
		QueryPerformanceFrequency(&frequency);
	}

	QueryPerformanceCounter(&counter);
	if(seconds != NULL)
		*seconds = counter.QuadPart / frequency.QuadPart;
	if(fractions != NULL)
		*fractions = (uint32)((((ULONGLONG) 0xffffffffUL) * (counter.QuadPart % frequency.QuadPart)) / frequency.QuadPart);
}

#else

#include <sys/time.h>

void a_get_current_time(uint32 *seconds, uint32 *fractions)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	if(seconds != NULL)
	    *seconds = tv.tv_sec;
	if(fractions != NULL)
		*fractions = tv.tv_usec * 1E-6 * (double) (uint32)~0;
}

#endif

double a_time_end(uint s, uint f)
{
	uint32 seconds, fractions;
	a_get_current_time(&seconds, &fractions);
	return (double)seconds - (double)s + ((double)fractions - (double)f) / (double)(0xffffffff);
}

#define A_EVENT_COUNT 64

FILE *a_file = NULL;

typedef struct{
	char *name;
	uint section;
}AEvent;

typedef struct{
	char *name;
	uint section;
}ACounter;

typedef struct{
	char *name;
	uint seconds;
	uint fractions;
	float acume_time;
	boolean active;
}ASubSection;

typedef struct{
	char *name;
	uint seconds;
	uint fractions;
	float acume_time;
	uint count;
	boolean active;
	ASubSection *sub_sections;
	uint sub_section_count;
}ASection;



AEvent		a_global_events[A_EVENT_COUNT];
uint		a_global_event_count = 0;
/*
ACounter	*a_global_counters = NULL;
uint		a_global_counter_count = 0;
*/
ASection	*a_global_sections = NULL;
uint		a_global_section_count = 0;
uint		a_global_section_active = -1;

uint		a_global_seconds = 0;
uint		a_global_fractions = 0;

void a_init(char *file_name)
{
	a_file = fopen(file_name, "wb");
	a_get_current_time(&a_global_seconds, &a_global_fractions);
}

uint a_section_allocate(char *section)
{
	uint i, j;
	for(i = 0; i < a_global_section_count; i++)
	{
		for(j = 0; section[j] != 0 && a_global_sections[i].name[j] == section[j]; j++);
		if(a_global_sections[i].name[j] == section[j])
			return i;
	}
	if(a_global_section_count % 16 == 0)
		a_global_sections = realloc(a_global_sections, (sizeof *a_global_sections) * (a_global_section_count + 16));
	a_global_sections[a_global_section_count].name = section;
	a_global_sections[a_global_section_count].seconds = 0;
	a_global_sections[a_global_section_count].fractions = 0;
	a_global_sections[a_global_section_count].acume_time = 0;
	a_global_sections[a_global_section_count].sub_sections = NULL;
	a_global_sections[a_global_section_count].sub_section_count = 0;
	a_global_sections[a_global_section_count].active = FALSE;
	a_global_section_count++;
	return a_global_section_count - 1;
}


void a_section_begin(uint id)
{
	a_global_section_active = id;
	a_global_sections[id].count++;
	a_global_sections[id].active = TRUE;
	a_get_current_time(&a_global_sections[id].seconds, &a_global_sections[id].fractions);
}

void a_section_end(uint id)
{
	a_global_sections[id].acume_time += a_time_end(a_global_sections[id].seconds, a_global_sections[id].fractions);
}


uint a_sub_section_allocate(uint id, char *section)
{
	uint i, j;
	for(i = 0; i < a_global_sections[id].sub_section_count; i++)
	{
		for(j = 0; section[j] != 0 && a_global_sections[id].sub_sections[i].name[j] == section[j]; j++);
		if(a_global_sections[id].sub_sections[i].name[j] == section[j])
			return i;
	}
	if(a_global_sections[id].sub_section_count % 16 == 0)
		a_global_sections[id].sub_sections = realloc(a_global_sections[id].sub_sections, (sizeof *a_global_sections[id].sub_sections) * (a_global_sections[id].sub_section_count + 16));
	a_global_sections[id].sub_sections[a_global_sections[id].sub_section_count].name = section;
	a_global_sections[id].sub_sections[a_global_sections[id].sub_section_count].fractions = 0;
	a_global_sections[id].sub_sections[a_global_sections[id].sub_section_count].acume_time = 0;
	a_global_sections[id].sub_sections[a_global_sections[id].sub_section_count].active = FALSE;
	a_global_sections[id].sub_section_count++;
	return a_global_sections[id].sub_section_count - 1;
}


void a_sub_section_begin(uint id)
{
	a_global_sections[a_global_section_active].sub_sections[id].active = TRUE;
	a_get_current_time(&a_global_sections[a_global_section_active].sub_sections[id].seconds, 
						&a_global_sections[a_global_section_active].sub_sections[id].fractions);
}

void a_sub_section_end(uint id)
{
	a_global_sections[a_global_section_active].sub_sections[id].acume_time += a_time_end(a_global_sections[a_global_section_active].sub_sections[id].seconds, 
																						a_global_sections[a_global_section_active].sub_sections[id].fractions);
}

void a_counter_add(uint id, float add)
{
	a_global_sections[a_global_section_active].sub_sections[id].acume_time += add;
}

void a_event(uint id, char *event)
{
	if(a_global_event_count >= A_EVENT_COUNT)
		return;
	a_global_events[a_global_event_count].name = event;
	a_global_events[a_global_event_count].section = id;
	a_global_event_count++;
}

void a_files_save_uint32(FILE *f, const uint32 data)
{
	fputc((data >> 24) & 0xFF, f);
	fputc((data >> 16) & 0xFF, f);
	fputc((data >> 8)  & 0xFF, f);
	fputc(data & 0xFF, f);
}

void a_files_save_real32(FILE *f, float data)
{
	union { uint32 integer; float real; } punt;
	punt.real = data;
	a_files_save_uint32(f, punt.integer);
}

void a_files_save_uint16(FILE *f, uint16 value)
{
	fputc(value % 256, f);
	fputc(value / 256, f);
}

void a_files_save_string(FILE *f, char *text)
{
	uint i; 
	printf("saving string %s\n", text);
	for(i = 0; text[i] != 0; i++)
		fputc(text[i], f);
	fputc(0, f);
}

uint a_files_load_uint32(FILE *f)
{
	uint32 data;
	data = ((uint32) fgetc(f)) << 24;
	data |= ((uint32) fgetc(f)) << 16;
	data |= ((uint32) fgetc(f)) << 8;
	data |= ((uint32) fgetc(f));
	return data;
}

float a_files_load_real32(FILE *f)
{
	union { uint integer; float real; } data;
	data.integer = ((uint32) fgetc(f)) << 24;
	data.integer |= ((uint32) fgetc(f)) << 16;
	data.integer |= ((uint32) fgetc(f)) << 8;
	data.integer |= ((uint32) fgetc(f));
	return data.real;
}

uint16 a_files_load_uint16(FILE *f)
{
	uint16 output;
	output = fgetc(f);
	output += 256 * fgetc(f);
	return output;
}

void a_files_load_string(FILE *f, char *text)
{
 	while((*text++ = fgetc(f)) != 0);
}

void a_frame_end()
{
	uint i, j;
	float fps;
	fps = a_time_end(a_global_seconds, a_global_fractions);

	fputc(TRUE, a_file); // continiue
	a_files_save_real32(a_file, fps);
	printf("FPS %f %u\n", fps, a_global_section_count);
	a_files_save_uint32(a_file, a_global_section_count);
	for(i = 0; i < a_global_section_count; i++)
	{
		printf("saving %f %u\n", a_global_sections[i].acume_time, a_global_sections[i].sub_section_count);
		a_files_save_real32(a_file, a_global_sections[i].acume_time);
		a_files_save_uint32(a_file, a_global_sections[i].sub_section_count);
		for(j = 0; j < a_global_sections[i].sub_section_count; j++)
		{
			printf("value %f\n", a_global_sections[i].sub_sections[j].acume_time);
			a_files_save_real32(a_file, a_global_sections[i].sub_sections[j].acume_time);
		}
	}

	a_files_save_uint32(a_file, a_global_event_count);
	for(i = 0; i < a_global_event_count; i++)
	{
		a_files_save_string(a_file,a_global_events[i].name);
		a_files_save_uint32(a_file, a_global_events[i].section);
	}

	for(i = 0; i < a_global_section_count; i++)
	{
		a_global_sections[i].acume_time = 0;
		a_global_sections[i].count = 0;
		a_global_sections[i].active = FALSE;
		for(j = 0 ; j < a_global_sections[i].sub_section_count; j++)
		{
			a_global_sections[i].sub_sections[j].acume_time = 0;
			a_global_sections[i].sub_sections[j].active = FALSE;
		}
	}
	a_global_event_count = 0;
	a_get_current_time(&a_global_seconds, &a_global_fractions);
}


void a_record_end()
{
	uint i, j, count = 1;
	float fps;
	fps = a_time_end(a_global_seconds, a_global_fractions);
	fputc(FALSE, a_file); // do not continiue

	count = 1;
	for(i = 0; i < a_global_section_count; i++)
		count += 1 + a_global_sections[i].sub_section_count;
	a_files_save_uint32(a_file, count);

	a_files_save_uint32(a_file, a_global_section_count);
	for(i = 0; i < a_global_section_count; i++)
	{
		a_files_save_string(a_file, a_global_sections[i].name);
		a_files_save_uint32(a_file, a_global_sections[i].sub_section_count);
		for(j = 0; j < a_global_sections[i].sub_section_count; j++)
			a_files_save_string(a_file, a_global_sections[i].sub_sections[j].name);
	}
	fclose(a_file);
	printf("saving files\n");
	a_file = NULL;
}
