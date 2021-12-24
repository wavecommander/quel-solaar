#include <stdio.h>
#include <stdlib.h>

#include "seduce.h"

typedef struct{
	char *text;
	uint checksum;
}SUILine;

SUILine *sui_translate_data_base = NULL;
uint sui_translate_data_base_count = 0;
uint sui_translate_data_base_allocated = 0;


char *sui_translate_add(char *text, uint checksum)
{
	uint i;
	for(i = 0; i < sui_translate_data_base_count; i++)
		if(sui_translate_data_base[i].checksum == checksum)
			return sui_translate_data_base[i].text;

	if(sui_translate_data_base_count == sui_translate_data_base_allocated)
	{
		sui_translate_data_base_allocated += 64;
		sui_translate_data_base = realloc(sui_translate_data_base, (sizeof *sui_translate_data_base) * sui_translate_data_base_allocated);
	}
	sui_translate_data_base[sui_translate_data_base_count].checksum = checksum;
	sui_translate_data_base[sui_translate_data_base_count].text = text;
	sui_translate_data_base_count++;
	return text;
}


char *sui_translate(char *text)
{
	uint i, checksum = 0;

	for(i = 0; text[i] != 0; i++)
		checksum += text[i] * ((i + 23) * (i + 65537));
	return sui_translate_add(text, checksum);
}


void sui_translate_load(char *file)
{
	FILE *f;
	boolean keep_going = TRUE;
	uint i, j, checksum;
	char text[4096], line[4096], *p;
	if((f = fopen(file, "r")) != NULL)
	{
		while(keep_going)
		{
			for(i = 0; i < 4095; i++)
			{
				text[i] = fgetc(f);
				if(text[i] == EOF || text[i] == '\n')
				{
					if(text[i] == EOF)
						keep_going = FALSE;
					break;
				}
			}
			text[i] = 0;
			if(1 == sscanf(text, "<id=%u", &checksum))
			{
				for(i = 0; i < 4095 && text[i] != '>'; i++);
				if(i < 4095)
				{
					i++;
					for(j = 0; i < 4095 && text[i] != 0; i++)
						line[j++] = text[i];
					line[j++] = 0;

					p = malloc((sizeof *line) * j);
					for(j = 0; line[j] != 0; j++)
					{
						if(line[j] == 36)
							p[j] = '\n';
						else
							p[j] = line[j];
					}
					p[j] = 0;
					sui_translate_add(p, checksum);
				}
			}
		}
	}
}

/*
*/
void sui_translate_save(char *file)
{
	char text[4096];
	uint i, j;
	FILE *f;
	if((f = fopen(file, "w")) != NULL)
	{
		for(i = 0; i < sui_translate_data_base_count; i++)
		{
			for(j = 0; sui_translate_data_base[i].text[j] != 0; j++)
			{
				if(sui_translate_data_base[i].text[j] == '\n')
					text[j] = 36;
				else
					text[j] = sui_translate_data_base[i].text[j];
			}
			text[j] = 0;
			fprintf(f, "<id=%u>%s\n", sui_translate_data_base[i].checksum, text);
		}
		fclose(f);
	}
}
