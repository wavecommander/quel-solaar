
#include <math.h>
#include <stdlib.h>
#include "forge.h"
#include "betray.h"
#include "relinquish.h"


typedef struct{
	float color[4];
	float pos[4];
	float start; 
	float end;
	float thickness;
	uint major_seed;
	uint minor_seed;
}SeduceLineImageGenLine;


void seduce_line_image_line_init(SeduceLineImageGenLine *line, uint major_seed, uint minor_seed, float start, float offset)
{
	float f;
	uint primary;
	primary = major_seed % 3;
	if(f_randf(minor_seed++) > 0.8)
		primary = (major_seed + minor_seed) % 3;
	line->color[primary] = 0.3 + f_randf(minor_seed++);
	f = f_randf(minor_seed++);
	line->color[(primary + 1) % 3] = f * f * line->color[primary];
	f = f_randf(minor_seed++);
	line->color[(primary + 2) % 3] = f * f * line->color[primary];
	line->start = start + f_randf(minor_seed++) * 0.1;
	line->end = line->start + 0.05 + f_randf(major_seed++) * 0.3;
	f = line->end - line->end;
	line->pos[0] = f_randf(major_seed++) + f_randnf(minor_seed++) * 0.1 + offset;
	line->pos[1] = line->pos[0] + f_randnf(major_seed++) * 0.3 + f_randnf(minor_seed++) * 0.1;
	line->pos[2] = line->pos[0] + f_randnf(major_seed++) * 0.3 + f_randnf(minor_seed++) * 0.1;
	line->pos[3] = line->pos[0] + f_randnf(major_seed++) * 0.3 + f_randnf(minor_seed++) * 0.1;

	f = f_randf(minor_seed++);
	line->thickness =  0.7 + f * f * 3.0;
	line->major_seed = major_seed;
	line->minor_seed = minor_seed;
}

boolean seduce_line_image_line_lookup(SeduceLineImageGenLine *line, float pos, float *place, float *thickness, float *color)
{
	static uint seed = 0;
	float f;
	if(pos < line->start)
		return FALSE;
	if(pos > line->end)
	{
		f = (line->pos[3] - line->pos[2]) / (line->end - line->start);
		line->start = line->end;
		line->end = line->start + 0.15 + 0.2 * f_randf(line->major_seed++);
		line->pos[0] = line->pos[3];
		line->pos[1] = line->pos[0] + f * (line->end - line->start);
		line->pos[2] = line->pos[1] + (line->end - line->start) * f_randnf(line->major_seed++) * 0.1 + f_randnf(line->minor_seed++) * 0.1;
		line->pos[3] = line->pos[1] + (line->end - line->start) * f_randnf(line->major_seed++) * 0.1 + f_randnf(line->minor_seed++) * 0.1;
	}
	*thickness = line->thickness;
	f = f_splinef((pos - line->start) / (line->end - line->start), line->pos[0], line->pos[1], line->pos[2], line->pos[3]);
	while(f < 0.0)
		f++;
	while(f > 1.0)
		f--;
	color[0] = line->color[0];
	color[1] = line->color[1];
	color[2] = line->color[2];
	color[3] = line->color[3];
	if(line->end > 1.0)
	{
		pos = (pos - line->start) / (1.0 - line->start);
		*thickness += pos * 50.0;
		pos *= pos * 0.3;
		color[0] += pos;
		color[1] += pos;
		color[2] += pos;
		color[3] += pos;
	}
	*place = f;
	return TRUE;
}

void seduce_line_image_line_render(float *buffer, uint resolution, SeduceLineImageGenLine *line)
{
	float pos, thickness, f, color[4];
	uint i, pixel, x_pos;
	for(i = 0; i < resolution; i++)
	{
		if(seduce_line_image_line_lookup(line, (float)i / resolution, &pos, &thickness, color))
		{
			pos *= (float)resolution;
			pos -= thickness * 0.5;
			f = pos - (float)((int)pos);
 			f = 1.0 - f;
			if(f > thickness)
				f = thickness;
			x_pos = (uint)pos * 4;
			pixel = x_pos + resolution * 4 * i;
			buffer[pixel + 0] += buffer[pixel + 0] + (color[0] - buffer[pixel + 0]) * f;
			buffer[pixel + 1] += buffer[pixel + 1] + (color[1] - buffer[pixel + 1]) * f;
			buffer[pixel + 2] += buffer[pixel + 2] + (color[2] - buffer[pixel + 2]) * f;
			buffer[pixel + 3] += buffer[pixel + 3] + (color[3] - buffer[pixel + 3]) * f;
			x_pos = (x_pos + 4)  % (resolution * 4);
			pixel = x_pos + resolution * 4 * i;
			thickness -= f;
			while(thickness > 1.0)
			{
				buffer[pixel + 0] += color[0];
				buffer[pixel + 1] += color[1];
				buffer[pixel + 2] += color[2];
				buffer[pixel + 3] += color[3];
				x_pos = (x_pos + 4)  % (resolution * 4);
				pixel = x_pos + resolution * 4 * i;
				thickness--;
			}
			buffer[pixel + 0] += buffer[pixel + 0] + (color[0] - buffer[pixel + 0]) * thickness;
			buffer[pixel + 1] += buffer[pixel + 1] + (color[1] - buffer[pixel + 1]) * thickness;
			buffer[pixel + 2] += buffer[pixel + 2] + (color[2] - buffer[pixel + 2]) * thickness;
			buffer[pixel + 3] += buffer[pixel + 3] + (color[3] - buffer[pixel + 3]) * thickness;
		}
	}
}



uint seduce_line_image_gen(uint resolution)
{
	SeduceLineImageGenLine line;
	float *buffer;
	uint i, j, seeds[5] = {3, 5, 17, 9, 16}; // 12 is cool
	static float offset[5] = {0.610417, -0.256250, 0.401042, 0.401042, 0.833333};
	buffer = malloc((sizeof *buffer) * resolution * resolution * 4);
	for(i = 0; i < resolution * resolution * 4; i++)
		buffer[i] = 0.0;
/*	{
		BInputState *input;
		input = betray_get_input_state();
		if(input->pointers[0].button[0])
		{
			i = (input->pointers[0].click_pointer_y[0] + 0.4) * 7.0;
			i %= 5;
			offset[i] = input->pointers[0].pointer_x;
			printf("static float offset[5] = {%f, %f, %f, %f, %f};", offset[0], offset[1], offset[2], offset[3], offset[4]);
		}
	}*/

	for(i = 0; i < 50; i++)
	{
		seduce_line_image_line_init(&line, seeds[i / 10], i, (float)(i / 10) / 5.0, offset[i / 10]);
	//	if(i / 10 == 4)
			seduce_line_image_line_render(buffer, resolution, &line);
	}
	for(i = resolution * (resolution - 1) * 4; i < resolution * resolution * 4; i++)
		buffer[i] = 1.0;
	i = r_texture_allocate(R_IF_RGBA_FLOAT32, resolution, resolution, 1, TRUE, TRUE, buffer);
	free(buffer);
	return i;
}

void sediuce_line_image_test()
{
	BInputState *input;
	static uint texture_id = -1;
	return;
	input = betray_get_input_state();
	if(texture_id == -1)
		texture_id = seduce_line_image_gen(1024);
	r_primitive_image(-0.4, -0.4, 0.0, 0.8, 0.8, 0.0, 0.0, 1.0, 1.0, texture_id, 1, 1, 1, 1);
	r_primitive_image(-0.5, 0.45, 0.0, 1.0, 0.001, 1.0, input->pointers[0].pointer_x, 0.0, input->pointers[0].pointer_x, texture_id, 1, 1, 1, 1);
	r_primitive_image(-0.5, -0.45, 0.0, 1.0, 0.001, 1.0, input->minute_time * 10.0, 0.0, input->minute_time * 10.0, texture_id, 1, 1, 1, 1);
	r_texture_free(texture_id);
}

typedef struct{
	RShader *circle_shader;
	uint circle_shader_location_start;
	uint circle_shader_location_size;
	uint circle_shader_location_add;
	uint circle_shader_location_multiply;
	uint circle_shader_location_time;
	void *circle_pool;
	uint texture_id;
}SeduceLineDraw;

SeduceLineDraw seduce_line_draw;

char *seduce_circle_shader_vertex = 
"attribute vec2 vertex;"
"uniform mat4 ModelViewProjectionMatrix;"
"uniform float start;"
"uniform float size;"
"uniform vec2 add;"
"uniform float time;"
"uniform vec2 multiply;"
"varying vec2 uv;"
"void main()"
"{"
"	vec4 center;"
"	center = ModelViewProjectionMatrix * vec4(0.0, 0.0, 0.0, 1.0);"
"	gl_Position = ModelViewProjectionMatrix * vec4(sin(start + vertex.x * size), cos(start + vertex.x * size), 0.0, 1.0);"
"	uv.x = add.x + vertex.y * multiply.x / center.z;"
"	uv.y = max(min(add.y + 0.02 * sin(vertex.y + time), 1.0), 0.0);"
"}";

 //min(1.0, max(add.y + sin(time + vertex * 7.0) * multiply.y, 0.0));

char *seduce_circle_shader_fragment = 
"uniform sampler2D tex;\n"
"varying vec2 uv;"
"void main()"
"{"
"	gl_FragColor = texture2D(tex, uv);"
"}";

#define SEDUCE_LINE_CIRCLE_SECTIONS_MAX 128

void seduce_draw_line_init()
{
	RFormats vertex_format_types[1] = {R_FLOAT};
	char buffer[2048];
	float *array;
	uint i, size = 2;
	seduce_line_draw.texture_id = seduce_line_image_gen(512);
	seduce_line_draw.circle_shader = r_shader_create_simple(buffer, 2048, seduce_circle_shader_vertex, seduce_circle_shader_fragment, "fade primitive");
	r_shader_texture_set(seduce_line_draw.circle_shader, 0, seduce_line_draw.texture_id); 
	seduce_line_draw.circle_shader_location_start = r_shader_uniform_location(seduce_line_draw.circle_shader, "start");
	seduce_line_draw.circle_shader_location_size = r_shader_uniform_location(seduce_line_draw.circle_shader, "size");
	seduce_line_draw.circle_shader_location_add = r_shader_uniform_location(seduce_line_draw.circle_shader, "add");
	seduce_line_draw.circle_shader_location_multiply = r_shader_uniform_location(seduce_line_draw.circle_shader, "multiply");
	seduce_line_draw.circle_shader_location_time = r_shader_uniform_location(seduce_line_draw.circle_shader, "time");
	r_shader_state_set_blend_mode(seduce_line_draw.circle_shader, GL_ONE, GL_ONE);
	

	array = malloc((sizeof *array) * 4 * SEDUCE_LINE_CIRCLE_SECTIONS_MAX);
	for(i = 0; i < SEDUCE_LINE_CIRCLE_SECTIONS_MAX; i++)
	{
		array[i * 4 + 0] = 2.0 * PI * (float)i / (float)SEDUCE_LINE_CIRCLE_SECTIONS_MAX;
		array[i * 4 + 2] = 2.0 * PI * (float)(i + 1) / (float)SEDUCE_LINE_CIRCLE_SECTIONS_MAX;
	}
	for(i = 0; i < SEDUCE_LINE_CIRCLE_SECTIONS_MAX / 2; i++)
	{
		array[i * 4 + 1] = 2.0 * PI * (float)i / (float)SEDUCE_LINE_CIRCLE_SECTIONS_MAX - PI;
		array[i * 4 + 3] = 2.0 * PI * (float)(i + 1) / (float)SEDUCE_LINE_CIRCLE_SECTIONS_MAX - PI;
	}
	for(; i < SEDUCE_LINE_CIRCLE_SECTIONS_MAX; i++)
	{
		array[i * 4 + 1] = 2.0 * PI * (float)(i - SEDUCE_LINE_CIRCLE_SECTIONS_MAX / 2) / (float)SEDUCE_LINE_CIRCLE_SECTIONS_MAX - PI;
		array[i * 4 + 3] = 2.0 * PI * (float)(i + 1 - SEDUCE_LINE_CIRCLE_SECTIONS_MAX / 2) / (float)SEDUCE_LINE_CIRCLE_SECTIONS_MAX - PI;
	}

	seduce_line_draw.circle_pool = r_array_allocate(SEDUCE_LINE_CIRCLE_SECTIONS_MAX * 2, &vertex_format_types, &size, 1, 0);
	r_array_load_vertex(seduce_line_draw.circle_pool, NULL, array, 0, SEDUCE_LINE_CIRCLE_SECTIONS_MAX * 2);

}

void seduce_draw_circle_new(float x, float y, float z,
					float x_normal, float y_normal, float z_normal, float radius, float start, float end, 
					float color_r, float color_g, float color_b, float color_a, float timer)
{
	uint i, count;
	float axis[3], other[3], m[16], cur_x, cur_y, last_x, last_y;
	BInputState *input;
	input = betray_get_input_state();
	count = 64;
	axis[0] = x_normal;
	axis[1] = y_normal;
	axis[2] = z_normal;
	other[0] = y;
	other[1] = -x;
	other[2] = -z;
	f_matrixzyf(m, NULL, axis, other);
	m[12] = x;
	m[13] = y;
	m[14] = z;
	r_matrix_push(NULL);
	r_matrix_matrix_mult(NULL, m);
	r_matrix_scale(NULL, radius, radius, radius);
	r_shader_set(seduce_line_draw.circle_shader);
	r_shader_float_set(seduce_line_draw.circle_shader, seduce_line_draw.circle_shader_location_start, start);
	r_shader_float_set(seduce_line_draw.circle_shader, seduce_line_draw.circle_shader_location_size, end - start);
	r_shader_vec2_set(seduce_line_draw.circle_shader, seduce_line_draw.circle_shader_location_add, radius * 1.37, 0.2 * sin(input->minute_time * PI * 2.0) + input->pointers[0].pointer_x);
	r_shader_float_set(seduce_line_draw.circle_shader, seduce_line_draw.circle_shader_location_time, input->minute_time * 30.0);
	r_shader_vec2_set(seduce_line_draw.circle_shader, seduce_line_draw.circle_shader_location_multiply, 0.4 * radius, input->pointers[0].pointer_x);
	r_array_section_draw(seduce_line_draw.circle_pool, NULL, R_PRIMITIVE_LINES, 0, 2 * SEDUCE_LINE_CIRCLE_SECTIONS_MAX);	
	r_primitive_line_flush();
	r_matrix_pop(NULL);
}

void seduce_draw_circle(float x, float y, float z,
					float x_normal, float y_normal, float z_normal, float radius, float start, float end, 
					float color_r, float color_g, float color_b, float color_a, float timer)
{
	uint i, count;
	float axis[3], other[3], m[16], cur_x, cur_y, last_x, last_y;
	count = 64;
	axis[0] = x_normal;
	axis[1] = y_normal;
	axis[2] = z_normal;
	other[0] = y;
	other[1] = -x;
	other[2] = -z;
	f_matrixzyf(m, NULL, axis, other);
	m[12] = x;
	m[13] = y;
	m[14] = z;
	r_matrix_push(NULL);
	r_matrix_matrix_mult(NULL, m);
	last_x = sin(((end - start) + start) * PI * 2.0);
	last_y = cos(((end - start) + start) * PI * 2.0);
	for(i = 0; i < count;)
	{
		i++;
		cur_x = sin(((float)i / (float)count * (end - start) + start) * PI * 2.0);
		cur_y = cos(((float)i / (float)count * (end - start) + start) * PI * 2.0);
		r_primitive_line_2d(cur_x * radius, cur_y * radius, last_x * radius, last_y * radius, color_r, color_g, color_b, color_a);
		last_x = cur_x;
		last_y = cur_y;
	}
	r_primitive_line_flush();
	r_matrix_pop(NULL);
}




void seduce_draw_spline(float x_a, float y_a, float z_a,
						float x_b, float y_b, float z_b,
						float x_c, float y_c, float z_c,
						float x_d, float y_d, float z_d,
						float start, float end, 
						float color_r, float color_g, float color_b, float color_a, float timer)
{
	uint i, count;
	float f = 0, f2 = 0, add;
	f = f2 = start;
	count = 24; 
	add = (end - start) * timer * 1.0 / (float)count;
	for(i = 0; i < count; i++)
	{
		f += add;
		r_primitive_line_3d(f_splinef(f, x_a, x_b, x_c, x_d),
							f_splinef(f, x_a, x_b, x_c, x_d),
							f_splinef(f, x_a, x_b, x_c, x_d),
							f_splinef(f2, x_a, x_b, x_c, x_d),
							f_splinef(f2, x_a, x_b, x_c, x_d),
							f_splinef(f2, x_a, x_b, x_c, x_d),
							color_r, color_g, color_b, color_a);
		f2 = f;
	}
	r_primitive_line_flush();
	r_matrix_pop(NULL);
}