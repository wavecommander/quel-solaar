
#include "st_matrix_operations.h"
#include "seduce.h"
#include "advance.h"
#include <math.h>

typedef enum{
	AO_VT_FPS,
	AO_VT_SECTION,
	AO_VT_SUBSECTION,
	AO_VT_COUNTER
}AOValueType;

typedef struct{
	uint type;
	float *values;
	float color[3];
	float sum;
	float start;
	float end;
	uint group;
	char name[32];
}AOValues;

AOValues *ao_values;
uint ao_values_count = 0;
uint ao_values_length = 0;

void ao_init_fake()
{
	float f = 0.5;
	uint i, j, k, seed = 0;
	ao_values_count = 48;
	ao_values = malloc((sizeof *ao_values) * ao_values_count);
	ao_values_length = 1000; 
	for(i = 0; i < ao_values_count; i++)
	{
		ao_values[i].type = AO_VT_SUBSECTION;
		if(i % 8 == 1)
			ao_values[i].type = AO_VT_SECTION;
		ao_values[i].group = (i - 1) / 8;
		if(i % 2)
			st_rgb(ao_values[i].color, (float)ao_values[i].group / (float)6, (((i - 1) % 8) / 8.0) * 0.5, 1.0);
		else
			st_rgb(ao_values[i].color, (float)ao_values[i].group / (float)6 + 0.01, (1.0 - ((i - 1) % 8) / 8.0) * 0.5, 0.7);
		ao_values[i].values = malloc((sizeof *ao_values[i].values) * ao_values_length);
		for(j = 0; j < ao_values_length; j++)
		{
			f += (get_rand(seed++) - f) * 0.4 * get_rand(i);
			ao_values[i].values[j] = (f - 0.5) * 1.0 + 0.5;
		}
		for(j = 1; j < ao_values_length - 1; j++)
		{
			ao_values[i].values[j] = (ao_values[i].values[j] + ao_values[i].values[j - 1] + ao_values[i].values[j + 2]) / 3.0;
		}
		ao_values[i].name[0] = 65;
		ao_values[i].name[1] = 66;
		ao_values[i].name[2] = 0;
	}
	for(i = 0; i < ao_values_count; i++)
	{
		if(ao_values[i].type == AO_VT_SECTION)
		{
			for(k = 0; k < ao_values_length; k++)
				ao_values[i].values[k] = 0;

			for(j = 0; j < ao_values_count; j++)
				if(i != j && ao_values[i].group == ao_values[j].group)
					for(k = 0; k < ao_values_length; k++)
						ao_values[i].values[k] += ao_values[j].values[k];
		}
	}
	ao_values[0].type = AO_VT_FPS;
	ao_values[3].type = AO_VT_COUNTER;
	ao_values[6].type = AO_VT_COUNTER;
}


void ao_init_load(char *file_name)
{
	FILE *file;
	char string[128];
	float f;
	uint i, j, k, count_a, count_b, read;

	file = fopen(file_name, "rb");

	read = fgetc(file);
	while(read && read != EOF)
	{
		a_files_load_real32(file);
		count_a = a_files_load_uint32(file);
		for(i = 0; i < count_a; i++)
		{
			a_files_load_real32(file);
			count_b = a_files_load_uint32(file);
			for(j = 0; j < count_b; j++)
				a_files_load_real32(file);
		}
		count_a = a_files_load_uint32(file);
		for(i = 0; i < count_a; i++)
		{
			a_files_load_string(file, string);
		}
		read = fgetc(file);
	}
	if(read == EOF)
	{
		ao_values_count = 0;
		return;
	}
	ao_values_count = a_files_load_uint32(file);
	ao_values = malloc((sizeof *ao_values) * ao_values_count);

	count_a = a_files_load_uint32(file);
	k = 0;
	ao_values[k].type = AO_VT_FPS;
	ao_values[k].values = NULL;
	ao_values[k].color[0] = 1;
	ao_values[k].color[1] = 1;
	ao_values[k].color[2] = 1;
	ao_values[k].sum = 0;
	ao_values[k].start = 0;
	ao_values[k].end = 0;
	ao_values[k].group = -1;
	ao_values[k].name[0] = 0;
	
	k = 1;
	for(i = 0; i < count_a; i++)
	{
		a_files_load_string(file, ao_values[k].name);

		ao_values[k].type = AO_VT_SECTION;
		ao_values[k].values = NULL;
		ao_values[k].color[0] = 1;
		ao_values[k].color[1] = 1;
		ao_values[k].color[2] = 1;
		ao_values[k].sum = 0;
		ao_values[k].start = 0;
		ao_values[k].end = 0;
		ao_values[k].group = i;
		k++;
		count_b = a_files_load_uint32(file);
		for(j = 0; j < count_b; j++)
		{
			a_files_load_string(file, ao_values[k].name);
			ao_values[k].type = AO_VT_SUBSECTION;
			ao_values[k].values = NULL;
			ao_values[k].color[0] = 1;
			ao_values[k].color[1] = 1;
			ao_values[k].color[2] = 1;
			if(j % 2)
				st_rgb(ao_values[k].color, (float)i / (float)6, ((j % count_b) / 8.0) * 0.5, 1.0);
			else
				st_rgb(ao_values[k].color, (float)i / (float)6 + 0.01, (1.0 - (j % count_b) / 8.0) * 0.5, 0.7);
			ao_values[k].sum = 0;
			ao_values[k].start = 0;
			ao_values[k].end = 0;
			ao_values[k].group = i;
			k++;
		}
	}


	fseek(file, 0, SEEK_SET);
	ao_values_length = 0;

	read = fgetc(file);
	while(read && read != EOF)
	{
		if(ao_values_length % 1024 == 0)
		{
			for(i = 0; i < ao_values_count; i++)
			{
				ao_values[i].values = realloc(ao_values[i].values, (sizeof *ao_values[i].values) *(ao_values_length + 1024));
				for(j = 0; j < 1024; j++)
					ao_values[i].values[ao_values_length + j] = 0;

			}
		}
		ao_values[0].values[ao_values_length] = f = a_files_load_real32(file);
		count_a = a_files_load_uint32(file);
		k = 1;
		for(i = 0; i < count_a; i++)
		{			
			ao_values[k++].values[ao_values_length] = a_files_load_real32(file);
			count_b = a_files_load_uint32(file);
			for(j = 0; j < count_b; j++)
			{
				ao_values[k++].values[ao_values_length] = a_files_load_real32(file);
			}
		}
		ao_values_length++;
		k = a_files_load_uint32(file);
		read = fgetc(file);
	}
	fclose(file);
}
/*
		ao_values[k].
	AO_VT_FPS,
	AO_VT_SECTION,
	AO_VT_SUBSECTION,
	AO_VT_COUNTER
*/

void ao_draw_text(float pos_x, float pos_y, float pos_z, char *text, float brightness)
{

}

void ao_draw(uint start, uint draw_length)
{
	static uint draw_section = -1, else_section;
	static uint alloc = 0;
	static float *array = NULL, *color = NULL;
	uint i, j, k;
	float f, f2, x, x2, y = -1, y2, length = 0, width, pie[3 * 3 * 24];


	if(alloc != draw_length)
	{
		if(array != NULL)
		{
			free(array);
			free(color);
		}
		array = malloc((sizeof *array) * draw_length * 3 * 4);
		color = malloc((sizeof *array) * draw_length * 3 * 4);
		alloc = draw_length;
	}

	for(j = start; j < start + draw_length - 1; j++) /*computing FPS scale*/ 
		length += ao_values[0].values[j];

	width = 0;
	for(i = 0; i < ao_values_count; i++)
	{
		ao_values[i].sum = 0;
		for(j = start; j < start + draw_length; j++)
		{
			ao_values[i].sum += ao_values[i].values[j];
		}
	//	ao_values[i].sum = 0.1;
		if(ao_values[i].type == AO_VT_SECTION)
			width += ao_values[i].sum;
	}
	x = -1;
	for(i = 0; i < ao_values_count; i++)
	{
		if(ao_values[i].type == AO_VT_SECTION)
		{
			ao_values[i].start = x;
			ao_values[i].end = x = x + 2.0 * ao_values[i].sum / width;
			f = 0;
			for(j = 0; j < ao_values_count; j++)
				if(j != i && ao_values[i].group == ao_values[j].group)
					f++;
			x2 = ao_values[i].start;
			for(j = 0; j < ao_values_count; j++)
			{
				if(j != i && ao_values[i].group == ao_values[j].group)
				{
					ao_values[j].start = x2;
					ao_values[j].end = x2 = x2 + (ao_values[i].end - ao_values[i].start) / f;
				}
			}
		}
	}

	x = -1.0;

	for(i = 0; i < ao_values_count; i++)
	{	
		if(ao_values[i].type == AO_VT_FPS)
		{
			y = -1;
			x = (((float)i - 0.5) / (float)ao_values_count) * 2.0 - 1.0;
			for(j = start; j < start + draw_length; j++)
			{
				y += ao_values[0].values[j] / length * 2.0;
				sui_draw_3d_line_gl(x, 0, y, x, ao_values[i].values[j] * 10.0, y, 1, 1, 1, 1);		
			}
		}
	}

	x = -1;
	for(i = 0; i < ao_values_count; i++)
	{	
		if(ao_values[i].type != AO_VT_SECTION)
		{
			glPushMatrix();
			sui_set_blend_gl(GL_ONE, GL_ONE);
			glTranslatef(ao_values[i].start, 0, 1);
			glScalef(-1, -1, -1);
			f = sui_text_line_length(1, SUI_T_SPACE, ao_values[1].name, -1);
			sui_draw_text(0, 0, (ao_values[i].start - ao_values[i].end) / f, SUI_T_SPACE, ao_values[i].name, ao_values[i].color[0], ao_values[i].color[1], ao_values[i].color[2], 1);
			sui_set_blend_gl(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glPopMatrix();
		}else
		{
			f = sui_text_line_length(1, SUI_T_SPACE, ao_values[1].name, -1);
			glPushMatrix();
			sui_set_blend_gl(GL_ONE, GL_ONE);
			glTranslatef(ao_values[i].start, 1.4 * (ao_values[i].start - ao_values[i].end) / f, 1);
			glScalef(-1, -1, -1);
			sui_draw_text(0, 0, (ao_values[i].start - ao_values[i].end) / f, SUI_T_SPACE, ao_values[i].name, ao_values[i].color[0], ao_values[i].color[1], ao_values[i].color[2], 1);
			sui_set_blend_gl(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glPopMatrix();
		}

		if(ao_values[i].type == AO_VT_SUBSECTION)
		{
			x = ao_values[i].start;
			x2 = ao_values[i].end;
			y = y2 = -1;
			for(j = 0; j < draw_length - 1 ; j++)
			{
				y2 += ao_values[0].values[j + start] / length * 2.0;
				array[j * 12 + 0] = x;
				array[j * 12 + 1] = ao_values[i].values[j + start] / ao_values[0].values[j + start];
				array[j * 12 + 2] = y; 

				array[j * 12 + 3] = x2;
				array[j * 12 + 4] = ao_values[i].values[j + start] / ao_values[0].values[j + start];
				array[j * 12 + 5] = y; 

				array[j * 12 + 6] = x2;
				array[j * 12 + 7] = ao_values[i].values[j + start + 1] / ao_values[0].values[j + start + 1]; 
				array[j * 12 + 8] = y2;

				array[j * 12 + 9] = x;
				array[j * 12 + 10] = ao_values[i].values[j + start + 1] / ao_values[0].values[j + start + 1]; 
				array[j * 12 + 11] = y2;

				f = (ao_values[i].values[j + start + 1] / ao_values[i].values[j + start]);
				if(f > 1.0)
					f = 2.0 - f;
				color[j * 12 + 0] = color[j * 12 + 3] = color[j * 12 + 6] = color[j * 12 + 9] = f * ao_values[i].color[0];
				color[j * 12 + 1] = color[j * 12 + 4] = color[j * 12 + 7] = color[j * 12 + 10] = f * ao_values[i].color[1];
				color[j * 12 + 2] = color[j * 12 + 5] = color[j * 12 + 8] = color[j * 12 + 11] = f * ao_values[i].color[2]; 
				y = y2;			
			}
			sui_set_color_array_gl(color, 4 * (draw_length - 1), 3);
			sui_draw_gl(GL_QUADS, array, 4 * (draw_length - 1), 3, ao_values[i].color[0], ao_values[i].color[1], ao_values[i].color[2], 0);
		}
		if(ao_values[i].type == AO_VT_COUNTER)
		{
			y = -1;
			x = (ao_values[i].start + ao_values[i].end) * 0.5;
			for(j = start; j < start + draw_length - 1; j++)
			{
				y2 = y + ao_values[0].values[j] / length * 2.0;
				sui_draw_3d_line_gl(x, ao_values[i].values[j + 1], y2, x, ao_values[i].values[j], y, ao_values[i].color[0] * 0.5 + 0.5, ao_values[i].color[1] * 0.5 + 0.5, ao_values[i].color[2] * 0.5 + 0.5, 1);
				y = y2;
			}
		}
	}

	for(i = 0; i < ao_values_count; i++)
	{	
		if(ao_values[i].type == AO_VT_SECTION)
		{
			x = (ao_values[i].start + ao_values[i].end) * 0.5;
			y = (ao_values[i].end - ao_values[i].start) * 0.5;
			f = 0;
			for(j = 0; j < ao_values_count; j++)
			{	
				if(ao_values[i].group == ao_values[j].group && ao_values[j].type == AO_VT_SUBSECTION)
				{
					f2 = ao_values[j].sum / ao_values[i].sum;
					for(k = 0; k < 24; k++)
					{	
						pie[k * 9 + 0] = x - sin((f + f2 * (float)k / 24.0) * 2.0 * PI) * y;
						pie[k * 9 + 1] = -y - cos((f + f2 * (float)k / 24.0) * 2.0 * PI) * y;
						pie[k * 9 + 2] = -1.0;
						
						pie[k * 9 + 3] = x - sin((f + f2 * (float)(k + 1) / 24.0) * 2.0 * PI) * y;
						pie[k * 9 + 4] = -y - cos((f + f2 * (float)(k + 1) / 24.0) * 2.0 * PI) * y;
						pie[k * 9 + 5] = -1.0;

						pie[k * 9 + 6] = x;
						pie[k * 9 + 7] = -y;
						pie[k * 9 + 8] = -1.0;
					}
					sui_draw_gl(GL_TRIANGLES, pie, 3 * 24, 3, ao_values[j].color[0], ao_values[j].color[1], ao_values[j].color[2], 1);
					f += f2;
				}
			}
		}
	}
	for(i = 0; i < ao_values_count; i++)
	{	
		if(ao_values[i].type == AO_VT_SECTION)
		{
			x = (ao_values[i].start + ao_values[i].end) * 0.5;
			y = (ao_values[i].end - ao_values[i].start) * 0.5;
			f = 0;
			for(j = 0; j < ao_values_count; j++)
			{	
				if(ao_values[i].group == ao_values[j].group && ao_values[j].type == AO_VT_SUBSECTION)
				{
					f2 = ao_values[j].sum / ao_values[i].sum;
					glPushMatrix();
					sui_set_blend_gl(GL_ONE, GL_ONE);
					glTranslatef(x - sin((f + f2 * 0.5) * 2.0 * PI) * y * 0.7, -y - cos((f + f2 * 0.5) * 2.0 * PI) * y * 0.7, -1.0 - f2 * 0.2);

					glScalef(f2 * -0.1, f2 * 0.1, 1);
					sui_draw_text(-0.5 * sui_text_line_length(1, SUI_T_SPACE, ao_values[j].name, -1), 0, 1, SUI_T_SPACE, ao_values[j].name, 1, 1, 1, 1);
					sui_set_blend_gl(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					glPopMatrix();
					f += f2;
				}
			}
		}
	}
}

void ao_input_handler(BInputState *input, void *user_pointer)
{
	static float speed = 0;
	static float start = 0;
	static float scale = 100;
	static float grab_scale = 100;
	if(input->mode == BAM_MAIN)
		return;

	start += speed;
	if(start < 0)
		start = 0;
	if(start > 1000 - scale - 2)
		start = 1000 - scale - 2;

	if(input->mode == BAM_DRAW)
	{
		glClearColor(0.1, 0.1, 0.1, 0.1);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glPushMatrix();
		sui_view_set(NULL);
	
		sui_draw_3d_line_gl(1, 0, 0, -1, 0, 0, 1, 0, 0, 0);
		sui_draw_3d_line_gl(0, 1, 0, 0, -1, 0, 0, 1, 0, 0);
		sui_draw_3d_line_gl(0, 0, 1, 0, 0, -1, 0, 0, 1, 0);
		ao_draw((uint)start, scale);
	}
	if(input->mode == BAM_EVENT)
	{
		if(input->mouse_button[1] && !input->last_mouse_button[1])
			sui_view_change_start(NULL, input);
		if(input->mouse_button[1])
			sui_view_change(NULL, input);
		if(input->mouse_button[0] && !input->mouse_button[1])
		{
			speed = input->delta_pointer_x * -100.0;
			scale = grab_scale * (1.0 + (input->click_pointer_y - input->pointer_y));
			if(scale > 1000 - 2)
				scale = 1000 - 2;
			if(scale < 10)
				scale = 10;
		}else
			grab_scale = scale;
	}	

	if(input->mode == BAM_DRAW)
	{
		glPopMatrix();
	}
}

void ao_context_update(void)
{
	glClearColor(0.1, 0.1, 0.1, 0.1);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_MODELVIEW);
	sui_set_blend_gl(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	sui_text_screen_mode_update();
}

int main(int argc, char **argv)
{
	betray_init(argc, argv, 720, 566, FALSE, "Adri Om");
	sui_init();
//	ao_init_fake();
	ao_init_load("advance_profile.bin");
	sui_view_init(NULL);
	glClearColor(0.1, 0.1, 0.1, 0.1);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	sui_load_settings("ao_config.cfg");
	betray_set_context_update_func(ao_context_update);
	betray_set_action_func(ao_input_handler, NULL);
	betray_launch_main_loop();
	return 0;
}
