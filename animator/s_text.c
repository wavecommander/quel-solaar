
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "seduce.h"

extern float sui_font_vertex_array[13098];
extern unsigned int *sui_font_ref_array[256];
extern unsigned int sui_font_ref_size[256];
extern float sui_font_letter_size[2048];

extern void sui_init_font_array();

unsigned int sui_draw_aa_samples = 10;

unsigned int	sui_screen_mode_size_x = 1500;
unsigned int	sui_screen_mode_size_y = 1100;
double			sui_screen_mode_view_fov = 1.0;
double			sui_screen_mode_view_near = 0.0005;
double			sui_screen_mode_view_far = 10.0;

#define SUI_TEXTURE_DRAW_CASH_SIZE 512
#define SUI_TEXTURE_LETTER_SIZE 2.0
#define SUI_TEXTURE_LETTER_BASE (0.5)


extern boolean	betray_set_screen_mode(uint x_size, uint y_size, boolean fullscreen);
extern double	betray_get_screen_mode(uint *x_size, uint *y_size, boolean *fullscreen);
extern void		betray_set_frustum_mode(double far, double near, double fov);
extern void		betray_get_frustum_mode(double *far, double *near, double *fov);

extern void sui_font_init();

void sui_text_screen_mode_update()
{
	betray_get_screen_mode(&sui_screen_mode_size_x, &sui_screen_mode_size_y, NULL);
	betray_get_frustum_mode(&sui_screen_mode_view_far, &sui_screen_mode_view_near, &sui_screen_mode_view_fov);
}

void sui_text_aa_mode(uint samples)
{
	sui_draw_aa_samples = samples;
}

void sui_text_line_draw_sample(float size, float spacing, const char *text, char length)
{
	unsigned int i = 0, j = 0, k;
	float f, f2, pos = 0;
	sui_font_init();
	glPushMatrix();
	glScalef(size, size, 1.0);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, sui_font_vertex_array);
	if(text[0] != 0)
	{
		while(i < length)
		{
			if(sui_font_ref_array[text[i]] != NULL)	
				glDrawElements(GL_TRIANGLES, sui_font_ref_size[text[i]], GL_UNSIGNED_INT, sui_font_ref_array[text[i]]);
			i++;
			if(text[i] == 0)
				break;
			f =  sui_font_letter_size[text[i - 1] * 8 + 4] - sui_font_letter_size[text[i] * 8 + 0];
			f2 = sui_font_letter_size[text[i - 1] * 8 + 5] - sui_font_letter_size[text[i] * 8 + 1];
			if(f2 > f)
				f = f2;
			f2 = sui_font_letter_size[text[i - 1] * 8 + 6] - sui_font_letter_size[text[i] * 8 + 2];
			if(f2 > f)
				f = f2;
			f2 = sui_font_letter_size[text[i - 1] * 8 + 7] - sui_font_letter_size[text[i] * 8 + 3];
			if(f2 > f)
				f = f2;
			glTranslatef(f + spacing, 0, 0);
		}
	}
	glPopMatrix();
}

void sui_text_line_draw(float size, float spacing, const char *text, uint length, uint samples)
{
	if(samples > 1)
	{
		uint i;
		float aspect;
		for(i = 0; i < samples; i++)
		{
			float x = 0, y = 0;
			x = (get_rand(i * 2 + 0) - 0.5) * 2 / (float)sui_screen_mode_size_x;
			y = (get_rand(i * 2 + 1) - 0.5) * 2 / (float)sui_screen_mode_size_x;
			aspect = (float)sui_screen_mode_size_y / (float)sui_screen_mode_size_x; 
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glFrustum((-sui_screen_mode_view_fov + x) * sui_screen_mode_view_near, (sui_screen_mode_view_fov + x) * sui_screen_mode_view_near, ((-sui_screen_mode_view_fov * aspect) + y) * sui_screen_mode_view_near, ((sui_screen_mode_view_fov * aspect) + y) * sui_screen_mode_view_near, sui_screen_mode_view_near, sui_screen_mode_view_far);
			glViewport(0, 0, sui_screen_mode_size_x, sui_screen_mode_size_y);
			glMatrixMode(GL_MODELVIEW);	
			sui_text_line_draw_sample(size, spacing, text, length);
		}
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(-sui_screen_mode_view_fov * sui_screen_mode_view_near, sui_screen_mode_view_fov * sui_screen_mode_view_near, -sui_screen_mode_view_fov * aspect * sui_screen_mode_view_near, sui_screen_mode_view_fov * aspect * sui_screen_mode_view_near, sui_screen_mode_view_near, sui_screen_mode_view_far);
		glViewport(0, 0, sui_screen_mode_size_x, sui_screen_mode_size_y);
		glMatrixMode(GL_MODELVIEW);	
	}
	sui_text_line_draw_sample(size, spacing, text, length);
}

#define SUI_T_SIZE 0.0125
#define SUI_T_SPACE 0.3

float sui_text_pos(float size, float spacing, const char *text, float left, float spaces, uint end)
{
	unsigned int i, j, k;
	float f, f2, length = 0;
	i = 0;
	if(text[0] != 0 && 0 != end)
	{
		while(TRUE)
		{
			i++;
			f =  sui_font_letter_size[text[i - 1] * 8 + 4] - sui_font_letter_size[text[i] * 8 + 0];
			f2 = sui_font_letter_size[text[i - 1] * 8 + 5] - sui_font_letter_size[text[i] * 8 + 1];
			if(f2 > f)
				f = f2;
			f2 = sui_font_letter_size[text[i - 1] * 8 + 6] - sui_font_letter_size[text[i] * 8 + 2];
			if(f2 > f)
				f = f2;
			f2 = sui_font_letter_size[text[i - 1] * 8 + 7] - sui_font_letter_size[text[i] * 8 + 3];
			if(f2 > f)
				f = f2;
			if(text[i] <= 32)
				length += f + spacing + left / (float)spaces;
			else
				length += f + spacing;
			if(text[i] == 0 || i == end)
				break;
		}
		return length;
	}
	return 0;
}

float sui_text_line_length(float size, float spacing, const char *text, uint end)
{
	return sui_text_pos(size, spacing, text, 0.0, 1.0, end) * size;
}

uint sui_compute_find_size(const char *text, float spacing, float size, boolean cut_words, uint *spaces, float *left)
{
	unsigned int i, j, k, end = 0;
	float f, f2, length = 0;
	i = 0;
	if(spaces != NULL)
		*spaces = 0;
	if(left != NULL)
		*left = 0;
	if(text[0] != 0)
	{
		while(TRUE)
		{
			i++;
			f = sui_font_letter_size[text[i - 1] * 8 + 4] - sui_font_letter_size[text[i] * 8 + 0];
			f2 = sui_font_letter_size[text[i - 1] * 8 + 5] - sui_font_letter_size[text[i] * 8 + 1];
			if(f2 > f)
				f = f2;
			f2 = sui_font_letter_size[text[i - 1] * 8 + 6] - sui_font_letter_size[text[i] * 8 + 2];
			if(f2 > f)
				f = f2;
			f2 = sui_font_letter_size[text[i - 1] * 8 + 7] - sui_font_letter_size[text[i] * 8 + 3];
			if(f2 > f)
				f = f2;
			length += f + spacing;
			if(text[i] < 33)
			{
				end = i;
				if(spaces != NULL)
					*spaces += 1;
				if(left != NULL)
					*left = length;
			}
			if(text[i] == 10)
			{
				if(left != NULL)
					*left = 0;
				return i;
			}
			if(text[i] == 0 || length > size)
			{
				if(spaces != NULL)
					*spaces -= 1;
				break;
			}
			if(cut_words)
				end = i;
		}
	}
	if(left != NULL)
		*left = size - *left;
	return end;
}


uint sui_text_block_line_end(const char *text, float spacing, float letter_size, float size)
{
	return sui_compute_find_size(text, spacing, size / letter_size, FALSE, NULL, NULL);
}

void sui_text_block_draw_sample(float letter_size, float letter_spacing, float line_size, float line_spacing, uint line_count, const char *text)
{
	unsigned char buf[1024];
	uint i, j, k = 0, add, pos = 0, spaces;
	float left, f, f2, letter_pos,array[SUI_TEXTURE_DRAW_CASH_SIZE * 4 * 2], uv[SUI_TEXTURE_DRAW_CASH_SIZE * 4 * 2];
	sui_font_init();
	glPushMatrix();
	glScalef(letter_size, letter_size, 1.0);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, sui_font_vertex_array);
	add = sui_compute_find_size(text, letter_spacing, line_size / letter_size, FALSE, &spaces, &left);
	for(i = 0; i < line_count; i++)
	{
		letter_pos = 0;
		for(j = 0; j < add && j < 1023; j++)
			buf[j] = text[pos + j];
		buf[j] = 0;
		glPushMatrix();
		j = 0;
		if(buf[0] != 0)
		{
			while(TRUE)
			{
				if(sui_font_ref_array[buf[j]] != NULL)
				{	
					uint tmp;
					float r;
					for(k = 0; k < sui_font_ref_size[buf[j]]; k++)
						r = sui_font_vertex_array[sui_font_ref_array[buf[j]][k] * 2];
					glDrawElements(GL_TRIANGLES, sui_font_ref_size[buf[j]], GL_UNSIGNED_INT, sui_font_ref_array[buf[j]]);
				}
				j++;
				if(buf[j] == 0)
					break;
				f = sui_font_letter_size[buf[j - 1] * 8 + 4] - sui_font_letter_size[buf[j] * 8 + 0];
				f2 = sui_font_letter_size[buf[j - 1] * 8 + 5] - sui_font_letter_size[buf[j] * 8 + 1];
				if(f2 > f)
					f = f2;
				f2 = sui_font_letter_size[buf[j - 1] * 8 + 6] - sui_font_letter_size[buf[j] * 8 + 2];
				if(f2 > f)
					f = f2;
				f2 = sui_font_letter_size[buf[j - 1] * 8 + 7] - sui_font_letter_size[buf[j] * 8 + 3];
				if(f2 > f)
					f = f2;
				if(sui_font_ref_array[buf[j]] != NULL || text[pos + add] == 0)
					f += letter_spacing;
				else
					f += letter_spacing + left / (float)spaces;
				glTranslatef(f, 0, 0);
			}
		}
		glPopMatrix();
		glTranslatef(0, -line_spacing, 0);
		pos += add;
		if(text[pos] == 0)
			break;
		pos += 1;
		add = sui_compute_find_size(&text[pos], letter_spacing, line_size / letter_size, FALSE, &spaces, &left);
	}
	glPopMatrix();
}

void sui_text_block_draw(float letter_size, float letter_spacing, float line_size, float line_spacing, uint line_count, const char *text)
{
	if(sui_draw_aa_samples > 1)
	{
		uint i;
		float aspect;
		for(i = 0; i < sui_draw_aa_samples; i++)
		{
			float x = 0, y = 0;
			x = (get_rand(i * 2 + 0) - 0.5) * 2 / (float)sui_screen_mode_size_x;
			y = (get_rand(i * 2 + 1) - 0.5) * 2 / (float)sui_screen_mode_size_x;
			aspect = (float)sui_screen_mode_size_x / (float)sui_screen_mode_size_y; 
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glFrustum((-sui_screen_mode_view_fov + x) * sui_screen_mode_view_near, (sui_screen_mode_view_fov + x) * sui_screen_mode_view_near, ((-sui_screen_mode_view_fov * aspect) + y) * sui_screen_mode_view_near, ((sui_screen_mode_view_fov * aspect) + y) * sui_screen_mode_view_near, sui_screen_mode_view_near, sui_screen_mode_view_far);
			glViewport(0, 0, sui_screen_mode_size_x, sui_screen_mode_size_y);
			glMatrixMode(GL_MODELVIEW);	
			sui_text_block_draw_sample(letter_size, letter_spacing, line_size, line_spacing, line_count, text);
		}
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(-sui_screen_mode_view_fov * sui_screen_mode_view_near, sui_screen_mode_view_fov * sui_screen_mode_view_near, -sui_screen_mode_view_fov * aspect * sui_screen_mode_view_near, sui_screen_mode_view_fov * aspect * sui_screen_mode_view_near, sui_screen_mode_view_near, sui_screen_mode_view_far);
		glViewport(0, 0, sui_screen_mode_size_x, sui_screen_mode_size_y);
		glMatrixMode(GL_MODELVIEW);	
	}
	sui_text_block_draw_sample(letter_size, letter_spacing, line_size, line_spacing, line_count, text);
}

uint sui_text_hit_test(float letter_size, float letter_spacing, const char *text, float left, float spaces, float pos_x, uint end)
{
	float f, f2, dist = 0;
	uint j;
	if(pos_x < 0)
		return 0;
	for(j = 0; j < end; j++)
	{
		if(text[j] == 0)
			return j;
		f = sui_font_letter_size[text[j] * 8 + 4] - sui_font_letter_size[text[j + 1] * 8 + 0];
		f2 = sui_font_letter_size[text[j] * 8 + 5] - sui_font_letter_size[text[j + 1] * 8 + 1];
		if(f2 > f)
			f = f2;
		f2 = sui_font_letter_size[text[j] * 8 + 6] - sui_font_letter_size[text[j + 1] * 8 + 2];
		if(f2 > f)
			f = f2;
		f2 = sui_font_letter_size[text[j] * 8 + 7] - sui_font_letter_size[text[j + 1] * 8 + 3];
		if(f2 > f)
			f = f2;
		if(sui_font_ref_array[text[j]] != NULL || text[end] == 0)
			f = f + letter_spacing;
		else
			f = f + letter_spacing + left / spaces;
		if(pos_x > dist - 0.5 && pos_x < f + dist - 0.5)
			return j;

		dist += f;
	}
	return j;
}

uint sui_text_block_hit_test(float letter_size, float letter_spacing, float line_size, float line_spacing, uint line_count, const char *text, float pos_x, float pos_y)
{
	char buf[4096];
	uint i, j, add, pos = 0, spaces, line;
	float left, f, f2, dist;
	sui_font_init();
	add = sui_compute_find_size(text, letter_spacing, line_size / letter_size, FALSE, &spaces, &left);

	line = (-pos_y + (letter_size * 2)) / (line_spacing * letter_size);
	if(line_count < line)
		return 0;
	for(i = 0; i < line_count && i < line; i++)
		pos += sui_compute_find_size(&text[pos], letter_spacing, line_size / letter_size, FALSE, &spaces, &left) + 1;
	if(text[pos] == 0)
		return pos - 1;
	add = sui_compute_find_size(&text[pos], letter_spacing, line_size / letter_size, FALSE, &spaces, &left) + 1;
	dist = 0;
	pos_x /= letter_size;
	return  sui_text_hit_test(letter_size, letter_spacing, &text[pos], left, spaces, pos_x, add) + pos;
}


void sui_text_block_pos(float letter_size, float letter_spacing, float line_size, float line_spacing, const char *text, uint curser, float *pos_x, float *pos_y)
{
	uint i, j, add, pos = 0, spaces;
	float left, length = 0, f, f2;
	for(i = 0; text[pos] != 0; i++)
	{
		pos++;
		add = sui_compute_find_size(&text[pos], letter_spacing, line_size / letter_size, FALSE, &spaces, &left);
		if(pos + add > curser && pos < curser)
		{
			for(j = pos; j < curser; j++)
			{
				f = sui_font_letter_size[text[j] * 8 + 4] - sui_font_letter_size[text[j + 1] * 8 + 0];
				f2 = sui_font_letter_size[text[j] * 8 + 5] - sui_font_letter_size[text[j + 1] * 8 + 1];
				if(f2 > f)
					f = f2;
				f2 = sui_font_letter_size[text[j] * 8 + 6] - sui_font_letter_size[text[j + 1] * 8 + 2];
				if(f2 > f)
					f = f2;
				f2 = sui_font_letter_size[text[j] * 8 + 7] - sui_font_letter_size[text[j + 1] * 8 + 3];
				if(f2 > f)
					f = f2;
				if(sui_font_ref_array[text[j + 1]] != NULL || text[pos + add] == 0)
					length += f + letter_spacing;
				else
					length += f + letter_spacing + left / (float)spaces;
			}
			if(pos_x != NULL)
				*pos_x = length * letter_size;
			if(pos_y != NULL)
				*pos_y = (float)i * -letter_size * line_spacing;
			return;
		}
		pos += add;
	}

}

uint sui_text_line_hit_test(float letter_size, float letter_spacing, const char *text, float pos_x)
{
	return sui_text_hit_test(letter_size, letter_spacing, text, 0, 1, pos_x / letter_size, -1);
}
