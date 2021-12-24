
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "forge.h"

#define FORGE_NXT	0x80
#define FORGE_SEQ2	0xc0
#define FORGE_SEQ3	0xe0
#define FORGE_SEQ4	0xf0
#define FORGE_SEQ5	0xf8
#define FORGE_SEQ6	0xfc
#define FORGE_BOM	0xfeff

uint32 f_utf8_to_uint32(char *c, uint *pos)
{
	uint i, bits, character_count = 1, high;
	uint32 out;

	if((c[*pos] & 0x80) == 0)
		high = (wchar_t)c[*pos];
	else if((c[*pos] & 0xe0) == FORGE_SEQ2)
	{
		character_count = 2;
		high = (wchar_t)(c[*pos] & 0x1f);
	}else if((c[*pos] & 0xf0) == FORGE_SEQ3)
	{
		character_count = 3;
		high = (wchar_t)(c[*pos] & 0x0f);
	}else if((c[*pos] & 0xf8) == FORGE_SEQ4)
	{
		character_count = 4;
		high = (wchar_t)(c[*pos] & 0x07);
	}else if((c[*pos] & 0xfc) == FORGE_SEQ5)
	{
		character_count = 5;
		high = (wchar_t)(c[*pos] & 0x03);
	}else if((c[*pos] & 0xfe) == FORGE_SEQ6)
	{
		character_count = 6;
		high = (wchar_t)(c[*pos] & 0x01);
	}else
	{
		(*pos)++;
		return 0;
	}
	out = 0;
	bits = 0;
	for(i = 1; i < character_count; i++)
	{
		out |= (wchar_t)(c[character_count - i] & 0x3f) << bits;
		bits += 6;		/* 6 low bits in every byte */
	}
	out |= high << bits;
	(*pos) += character_count;
	return out;
}

uint f_uint32_to_utf8(uint32 character, char *out)
{
	uint8 bytes[4];
	bytes[0] = (character >> 24) & 0xFF;
	bytes[1] = (character >> 16) & 0xFF;
	bytes[2] = (character >> 8) & 0xFF;
	bytes[3] = character & 0xFF;
	if(character <= 0x0000007f)
	{
		out[0] = bytes[3];
		return 1;
	}else if(character <= 0x000007ff)
	{
		out[1] = FORGE_NXT | bytes[3] & 0x3f;
		out[0] = FORGE_SEQ2 | (bytes[3] >> 6) | ((bytes[2] & 0x07) << 2);
		return 2;
	}else if(character <= 0x0000ffff)
	{
		out[2] = FORGE_NXT | bytes[3] & 0x3f;
		out[1] = FORGE_NXT | (bytes[3] >> 6) | ((bytes[2] & 0x0f) << 2);
		out[0] = FORGE_SEQ3 | ((bytes[2] & 0xf0) >> 4);
		return 3;
	}else if(character <= 0x001fffff)
	{
		out[3] = FORGE_NXT | bytes[3] & 0x3f;
		out[2] = FORGE_NXT | (bytes[3] >> 6) | ((bytes[2] & 0x0f) << 2);
		out[1] = FORGE_NXT | ((bytes[2] & 0xf0) >> 4) | ((bytes[1] & 0x03) << 4);
		out[0] = FORGE_SEQ4 | ((bytes[1] & 0x1f) >> 2);
		return 4;
	}else if(character <= 0x03ffffff)
	{
		out[4] = FORGE_NXT | bytes[3] & 0x3f;
		out[3] = FORGE_NXT | (bytes[3] >> 6) | ((bytes[2] & 0x0f) << 2);
		out[2] = FORGE_NXT | ((bytes[2] & 0xf0) >> 4) | ((bytes[1] & 0x03) << 4);
		out[1] = FORGE_NXT | (bytes[1] >> 2);
		out[0] = FORGE_SEQ5 | bytes[0] & 0x03;
		return 5;
	}else /* if (*w <= 0x7fffffff) */
	{
		out[5] = FORGE_NXT | bytes[3] & 0x3f;
		out[4] = FORGE_NXT | (bytes[3] >> 6) | ((bytes[2] & 0x0f) << 2);
		out[3] = FORGE_NXT | (bytes[2] >> 4) | ((bytes[1] & 0x03) << 4);
		out[2] = FORGE_NXT | (bytes[1] >> 2);
		out[1] = FORGE_NXT | bytes[0] & 0x3f;
		out[0] = FORGE_SEQ6 | ((bytes[0] & 0x40) >> 6);
		return 6;
	}
}

uint f_find_next_word(char *text)
{
	uint i;
	for(i = 1; text[i] > 32; i++);
	return i;
}

boolean f_find_word_compare(char *text_a, char *text_b)
{
	uint i;
	for(i = 0; text_a[i] == text_b[i] && text_a[i] > 32 && text_b[i] > 32; i++);
	return (text_a[i] < 32 && text_b[i] < 32);
}

boolean f_text_compare(char *text_a, char *text_b)
{
	uint i;
	for(i = 0; text_a[i] == text_b[i] && text_a[i] != 0; i++);
	return text_a[i] == text_b[i];
}

uint f_text_copy(uint length, char *dest, char *source)
{
	uint i;
	for(i = 0; i < length && source[i] != 0; i++)
		dest[i] = source[i];
	if(i == length)
		return 0;
	dest[i] = source[i];
	return i;
}


char *f_text_copy_allocate(char *source)
{
	uint i;
	char *text;
	for(i = 0; source[i] != 0; i++);
	text = malloc((sizeof *text) * (i + 1));
	for(i = 0; source[i] != 0; i++)
		text[i] = source[i];
	text[i] = 0;
	return text;
}

uint f_word_copy(uint length, char *dest, char *source)
{
	uint i = 0, j;
	for(j = 0; j < length && source[j] < 32; j++);
	while( i < length && source[j] > 32)
		dest[i++] = source[j++];
	if(i == length)
		return 0;
	dest[i] = 0;
	return j;
}


uint f_text_copy_until(uint length, char *dest, char *source, char *until)
{
	uint i = 0, j, k, l;
	for(j = 0; j < length && source[j] < 32; j++);
	while( i < length && source[j] != 0)
	{
		for(k = 0; source[j + k] == until[k] && until[k] != 0 ; k++);
		if(until[k] == 0)
		{
			for(i--; dest[i] < 32; i--);
			dest[i] = 0;
			return j + k;
		}
		else
			dest[i++] = source[j++];
	}
	return 0;
}

boolean	f_text_filter(char *text, char *filter)
{
	uint i, j;
	for(i = 0; text[i] != 0; i++)
	{
		if(text[i] == *filter)
		{
			for(j = 1; text[j] != 0 && text[i + j] == filter[j]; j++);
			if(filter[j] == 0)
				return TRUE;
		}
	}
	return FALSE;
}

char *f_text_load(char *file_name)
{
	char *buffer;
	uint size;
	FILE *f;
	f = fopen(file_name, "r");
	if(f == NULL)
	{
		return NULL;
	}
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	rewind(f);
	buffer = malloc(size + 1);
	size = fread(buffer, 1, size, f);
	fclose(f);
	buffer[size] = 0;
	return buffer;
}