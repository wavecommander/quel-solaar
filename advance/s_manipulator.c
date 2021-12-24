#include <math.h>
#include "seduce.h"


struct{
	float		*active_vertex;
	float		*active_vertex_shadow;
	float		*active_vertex_color;
	float		*move_vertex;
	float		*move_vertex_shadow;
	float		*move_vertex_color;
	float		*tag_select;
	float		*tag_select_shadow;
	float		*tag_select_color;
	float		*tag_unselect;
	float		*tag_unselect_shadow;
	float		*tag_unselect_color;
	float		*tri;
	float		*tri_normal;
	uint		tri_count;
	float		*quad;
	float		*quad_normal;
	float		*wire;
	uint		wire_count;
	uint		quad_count;
	uint		surface_version;
}GlobalOverlay;

void sui_do_init(void)
{
	static boolean init = FALSE;
	uint i;
	float temp, square[8] = {0.006, 0.01, 0.003, 0.038, -0.003, 0.038, -0.006, 0.01};
	if(init)
		return;
	init = TRUE;
	GlobalOverlay.active_vertex = malloc((sizeof * GlobalOverlay.active_vertex) * 4 * 4 * 2);
	GlobalOverlay.active_vertex_shadow = malloc((sizeof * GlobalOverlay.active_vertex_shadow) * 16 * 4 * 2);
	GlobalOverlay.active_vertex_color = malloc((sizeof * GlobalOverlay.active_vertex_color) * 16 * 4 * 4);
	for(i = 0; i < 4; i++)
	{
		square[0] = 0.013 * sin((float)i * 3.14 * 0.5 - 0.4);
		square[1] = 0.013 * cos((float)i * 3.14 * 0.5 - 0.4);
		square[2] = 0.045 * sin((float)i * 3.14 * 0.5 - 0.05);
		square[3] = 0.045 * cos((float)i * 3.14 * 0.5 - 0.05);
		square[4] = 0.045 * sin((float)i * 3.14 * 0.5 + 0.05);
		square[5] = 0.045 * cos((float)i * 3.14 * 0.5 + 0.05);
		square[6] = 0.013 * sin((float)i * 3.14 * 0.5 + 0.4);
		square[7] = 0.013 * cos((float)i * 3.14 * 0.5 + 0.4);
		sui_draw_set_vec2(GlobalOverlay.active_vertex, i * 4 + 0, square[0], square[1]);
		sui_draw_set_vec2(GlobalOverlay.active_vertex, i * 4 + 1, square[2], square[3]);
		sui_draw_set_vec2(GlobalOverlay.active_vertex, i * 4 + 2, square[4], square[5]);
		sui_draw_set_vec2(GlobalOverlay.active_vertex, i * 4 + 3, square[6], square[7]);
		sui_create_shadow_edge(0.005, 4, &GlobalOverlay.active_vertex_shadow[i * 4 * 2], &GlobalOverlay.active_vertex_color[i * 4 * 4], square);
	}
	GlobalOverlay.move_vertex = malloc((sizeof * GlobalOverlay.move_vertex) * 4 * 8 * 2);
	GlobalOverlay.move_vertex_shadow = malloc((sizeof * GlobalOverlay.move_vertex_shadow) * 16 * 8 * 2);
	GlobalOverlay.move_vertex_color = malloc((sizeof * GlobalOverlay.move_vertex_color) * 16 * 8 * 4);
	for(i = 0; i < 8; i++)
	{
		square[0] = 0.02 * sin((float)i * 3.14 * 0.25);
		square[1] = 0.02 * cos((float)i * 3.14 * 0.25);
		square[2] = 0.025 * sin((float)i * 3.14 * 0.25);
		square[3] = 0.025 * cos((float)i * 3.14 * 0.25);
		square[4] = 0.025 * sin((float)i * 3.14 * 0.25 + 0.3);
		square[5] = 0.025 * cos((float)i * 3.14 * 0.25 + 0.3);
		square[6] = 0.017 * sin((float)i * 3.14 * 0.25 + 0.3);
		square[7] = 0.017 * cos((float)i * 3.14 * 0.25 + 0.3);
		sui_draw_set_vec2(GlobalOverlay.move_vertex, i * 4 + 0, square[0], square[1]);
		sui_draw_set_vec2(GlobalOverlay.move_vertex, i * 4 + 1, square[2], square[3]);
		sui_draw_set_vec2(GlobalOverlay.move_vertex, i * 4 + 2, square[4], square[5]);
		sui_draw_set_vec2(GlobalOverlay.move_vertex, i * 4 + 3, square[6], square[7]);
		sui_create_shadow_edge(0.005, 4, &GlobalOverlay.move_vertex_shadow[i * 4 * 2 * 4], &GlobalOverlay.move_vertex_color[i * 4 * 4 * 4], square);
	}

	GlobalOverlay.tag_select = malloc((sizeof * GlobalOverlay.tag_select) * 4 * 6 * 2);
	GlobalOverlay.tag_select_shadow = malloc((sizeof * GlobalOverlay.tag_select_shadow) * 16 * 6 * 2);
	GlobalOverlay.tag_select_color = malloc((sizeof * GlobalOverlay.tag_select_color) * 16 * 6 * 4);
	for(i = 0; i < 6; i++)
	{
		square[0] = 0.01 * sin((float)i * 3.14 * 0.33333333);
		square[1] = 0.01 * cos((float)i * 3.14 * 0.33333333);
		square[2] = 0.02 * sin((float)i * 3.14 * 0.33333333 + 0.2);
		square[3] = 0.02 * cos((float)i * 3.14 * 0.33333333 + 0.2);
		square[4] = 0.02 * sin((float)i * 3.14 * 0.33333333 + 0.3);
		square[5] = 0.02 * cos((float)i * 3.14 * 0.33333333 + 0.3);
		square[6] = 0.01 * sin((float)i * 3.14 * 0.33333333 + 0.5);
		square[7] = 0.01 * cos((float)i * 3.14 * 0.33333333 + 0.5);
		sui_draw_set_vec2(GlobalOverlay.tag_select, i * 4 + 0, square[0], square[1]);
		sui_draw_set_vec2(GlobalOverlay.tag_select, i * 4 + 1, square[2], square[3]);
		sui_draw_set_vec2(GlobalOverlay.tag_select, i * 4 + 2, square[4], square[5]);
		sui_draw_set_vec2(GlobalOverlay.tag_select, i * 4 + 3, square[6], square[7]);
		sui_create_shadow_edge(0.005, 4, &GlobalOverlay.tag_select_shadow[i * 4 * 2], &GlobalOverlay.tag_select_color[i * 4 * 4], square);
	}

	GlobalOverlay.tag_unselect = malloc((sizeof * GlobalOverlay.tag_unselect) * 4 * 6 * 2);
	GlobalOverlay.tag_unselect_shadow = malloc((sizeof * GlobalOverlay.tag_unselect_shadow) * 16 * 6 * 2);
	GlobalOverlay.tag_unselect_color = malloc((sizeof * GlobalOverlay.tag_unselect_color) * 16 * 6 * 4);
	for(i = 0; i < 6; i++)
	{
		square[0] = 0.01 * sin((float)i * 3.14 * 0.33333333);
		square[1] = 0.01 * cos((float)i * 3.14 * 0.33333333);
		square[2] = 0.013 * sin((float)i * 3.14 * 0.33333333);
		square[3] = 0.013 * cos((float)i * 3.14 * 0.33333333);
		square[4] = 0.013 * sin((float)i * 3.14 * 0.33333333 + 0.5);
		square[5] = 0.013 * cos((float)i * 3.14 * 0.33333333 + 0.5);
		square[6] = 0.01 * sin((float)i * 3.14 * 0.33333333 + 0.5);
		square[7] = 0.01 * cos((float)i * 3.14 * 0.33333333 + 0.5);
		sui_draw_set_vec2(GlobalOverlay.tag_unselect, i * 4 + 0, square[0], square[1]);
		sui_draw_set_vec2(GlobalOverlay.tag_unselect, i * 4 + 1, square[2], square[3]);
		sui_draw_set_vec2(GlobalOverlay.tag_unselect, i * 4 + 2, square[4], square[5]);
		sui_draw_set_vec2(GlobalOverlay.tag_unselect, i * 4 + 3, square[6], square[7]);
		sui_create_shadow_edge(0.005, 4, &GlobalOverlay.tag_unselect_shadow[i * 4 * 2], &GlobalOverlay.tag_unselect_color[i * 4 * 4], square);
	}
	
	GlobalOverlay.tri_normal = NULL;
	GlobalOverlay.tri_count = 0;
	GlobalOverlay.quad = NULL;
	GlobalOverlay.quad_normal = NULL;
	GlobalOverlay.quad_count = 0;
	GlobalOverlay.wire_count = 0;
}


void sui_manipulator_line_draw(float *pos_0, float *pos_1, float width_0, float width_1, float red, float green, float blue)
{
	float vec[2], f, square[8], vertex_shadow[4 * 4 * 2], vertex_color[4 * 4 * 4];
	vec[0] = pos_0[0] - pos_1[0];
	vec[1] = pos_0[1] - pos_1[1];
	f = sqrt(vec[0] * vec[0] + vec[1] * vec[1]);
	if(f < 0.06)
		return;
	vec[0] /= f;
	vec[1] /= f;
	square[0] = pos_0[0] - vec[1] * width_0 - vec[0] * 0.03;
	square[1] = pos_0[1] + vec[0] * width_0 - vec[1] * 0.03;
	square[2] = pos_0[0] + vec[1] * width_0 - vec[0] * 0.03;
	square[3] = pos_0[1] - vec[0] * width_0 - vec[1] * 0.03;
	square[4] = pos_1[0] + vec[1] * width_1 + vec[0] * 0.03;
	square[5] = pos_1[1] - vec[0] * width_1 + vec[1] * 0.03;
	square[6] = pos_1[0] - vec[1] * width_1 + vec[0] * 0.03;
	square[7] = pos_1[1] + vec[0] * width_1 + vec[1] * 0.03;
	sui_draw_gl(GL_QUADS, square, 4, 2, red, green, blue, 0.8);
	sui_create_shadow_edge(0.01, 4, vertex_shadow, vertex_color, square);
	sui_set_color_array_gl(vertex_color, 4 * 4, 4);
	sui_draw_gl(GL_QUADS, vertex_shadow, 4 * 4, 2, 1.0, 1.0, 1.0, 0.8);
}

void sui_manipulator_vertex_draw(float *pos, boolean active, float red, float green, float blue)
{
	float f = 0.3;
	if(active)
		f = 1.0;
	sui_do_init();
	glPushMatrix();
	glTranslated(pos[0], pos[1], 0);
	sui_draw_gl(GL_QUADS, GlobalOverlay.move_vertex, 32, 2, red * f, green * f, blue * f, f * 0.8);
	if(active)
	{
		sui_set_color_array_gl(GlobalOverlay.move_vertex_color, 32 * 4, 4);
		sui_draw_gl(GL_QUADS, GlobalOverlay.move_vertex_shadow, 32 * 4, 2, red, green, blue, 0.8);
	}
	glPopMatrix();
}

void sui_manipulator_point_draw(float *pos, float red, float green, float blue)
{
	static float center_vertex[24 * 3 * 2], vertex_shadow[24 * 4 * 2], vertex_color[24 * 4 * 4];
	static boolean init = FALSE;
	if(!init)
	{
		uint i;
		float square[24 * 2];
		for(i = 0; i < 24; i++)
		{
			center_vertex[i * 6 + 0] = square[i * 2 + 0] = sin((float)i * PI * 2.0 / 24.0) * 0.01;
			center_vertex[i * 6 + 1] = square[i * 2 + 1] = cos((float)i * PI * 2.0 / 24.0) * 0.01;
			center_vertex[i * 6 + 2] = sin((float)(i + 1) * PI * 2.0 / 24.0) * 0.01;
			center_vertex[i * 6 + 3] = cos((float)(i + 1) * PI * 2.0 / 24.0) * 0.01;
			center_vertex[i * 6 + 4] = 0.0;
			center_vertex[i * 6 + 5] = 0.0;
		}
		sui_create_shadow_edge(0.01, 24, vertex_shadow, vertex_color, square);
		init = TRUE;
	}
	glPushMatrix();
	glTranslated(pos[0], pos[1], 0);
	sui_draw_gl(GL_TRIANGLES, center_vertex, 24 * 3, 2, red, green, blue, 0.8);
	sui_set_color_array_gl(vertex_color, 24 * 4, 4);
	sui_draw_gl(GL_QUADS, vertex_shadow, 24 * 4, 2, red, green, blue, 0.8);
	glPopMatrix();
}

boolean sui_manipulator_point_grab(BInputState *input, SViewData *v, float *pos)
{
	float tmp[3];
	sui_view_projection_screenf(v, tmp, pos[0], pos[1], pos[2]);
	tmp[0] -= input->pointer_x;
	tmp[1] -= input->pointer_y;
	if(tmp[0] * tmp[0] + tmp[1] * tmp[1] < 0.1 * 0.1)
		return TRUE;
	return FALSE;
}

void sui_manipulator_point_move(BInputState *input, SViewData *v, float *pos)
{
	sui_view_projection_vertexf(v, pos, pos, input->pointer_x, input->pointer_y);
}

boolean sui_manipulator_point(BInputState *input, SViewData *v, float *pos, void *id, float red, float green, float blue, char *name)
{
	static void *grab = NULL;
	float tmp[3], f;
	if(input->mode == BAM_EVENT && grab == NULL && input->mouse_button[0] && !input->last_mouse_button[0])
	{
		sui_view_projection_screenf(v, tmp, pos[0], pos[1], pos[2]);
		tmp[0] -= input->pointer_x;
		tmp[1] -= input->pointer_y;
		if(tmp[0] * tmp[0] + tmp[1] * tmp[1] < 0.02 * 0.02)
			grab = id;
	}
	if(input->mode == BAM_EVENT && !input->mouse_button[0])
		grab = NULL;
	if(input->mode == BAM_EVENT && grab == id)
		sui_view_projection_vertexf(v, pos, pos, input->pointer_x, input->pointer_y);

	if(input->mode == BAM_DRAW)
	{
		sui_view_projection_screenf(NULL, tmp, pos[0], pos[1], pos[2]);
		sui_manipulator_point_draw(tmp, red, green, blue);
		if(name != NULL)
		{
			f = sqrt(tmp[0] * tmp[0] + tmp[1] * tmp[1]);
			sui_draw_2d_line_gl(tmp[0] + tmp[0] / f * 0.02, tmp[1] + tmp[1] / f * 0.02, tmp[0] + tmp[0] / f * 0.18, tmp[1] + tmp[1] / f * 0.18, red, green, blue, 0.2);

			if(tmp[1] / f > 0.7 || tmp[1] / f < -0.7)
			{
				sui_draw_text(tmp[0] + tmp[0] / f * 0.2 - sui_text_line_length(SUI_T_SIZE * 0.5, SUI_T_SPACE, name, -1), tmp[1] + tmp[1] / f * 0.2, SUI_T_SIZE, SUI_T_SPACE, name, red, green, blue, 0.2);
			}
			else if(tmp[0] < 0)
				sui_draw_text(tmp[0] + tmp[0] / f * 0.2 - sui_text_line_length(SUI_T_SIZE, SUI_T_SPACE, name, -1), tmp[1] + tmp[1] / f * 0.2, SUI_T_SIZE, SUI_T_SPACE, name, red, green, blue, 0.2);
			else
				sui_draw_text(tmp[0] + tmp[0] / f * 0.2, tmp[1] + tmp[1] / f * 0.2, SUI_T_SIZE, SUI_T_SPACE, name,  red, green, blue, 0.2);


		}
	}
	return grab == id;
}

boolean sui_manipulator_normal(BInputState *input, SViewData *v, float *pos, float *normal, void *id, float red, float green, float blue)
{
	static void *grab = NULL;
	float tmp[3], axis_pos[6][3], place[3], dist = 0.3, f;
	uint i, axis = -1;
	sui_view_projection_screenf(v, tmp, pos[0], pos[1], pos[2]);
	dist *= -tmp[2];
	place[0] = pos[0] + normal[0] * dist;
	place[1] = pos[1] + normal[1] * dist;
	place[2] = pos[2] + normal[2] * dist;
	if(input->mode == BAM_EVENT && grab == NULL && input->mouse_button[0] && !input->last_mouse_button[0])
	{
		sui_view_projection_screenf(v, tmp, place[0], place[1], place[2]);
		tmp[0] -= input->pointer_x;
		tmp[1] -= input->pointer_y;
		if(tmp[0] * tmp[0] + tmp[1] * tmp[1] < 0.02 * 0.02)
			grab = id;
	}
	if(input->mode == BAM_EVENT && !input->mouse_button[0])
		grab = NULL;

	for(i = 0; i < 6; i++)
	{
		tmp[0] = pos[0];
		tmp[1] = pos[1];
		tmp[2] = pos[2];
		tmp[i / 2] += ((float)(i % 2) * 2.0 - 1.0) * dist * 1.3;
		sui_view_projection_screenf(NULL, axis_pos[i], tmp[0], tmp[1], tmp[2]);
	}
	for(i = 0; i < 3; i++)
	{
		tmp[0] = axis_pos[i * 2][0] - axis_pos[i * 2 + 1][0];
		tmp[1] = axis_pos[i * 2][1] - axis_pos[i * 2 + 1][1];
		if(tmp[0] * tmp[0] + tmp[1] * tmp[1] < 0.04 * 0.04)
			axis = i;
	}

	if(input->mode == BAM_EVENT && grab == id)
	{
		sui_view_projection_vertexf(v, place, place, input->pointer_x, input->pointer_y);
		normal[0] = place[0] - pos[0];
		normal[1] = place[1] - pos[1];
		normal[2] = place[2] - pos[2];
		f = normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2];
		if(axis < 3/* && f > dist * 0.9*/)
		{	
			normal[axis] = 0.0;
			f = normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2];
		}
		f = sqrt(f);
		normal[0] /= f;	
		normal[1] /= f;	
		normal[2] /= f;
	}
	if(input->mode == BAM_DRAW)
	{
		sui_view_projection_screenf(v, place, pos[0], pos[1], pos[2]);
		sui_view_projection_screenf(v, tmp, pos[0] + normal[0] * dist, 
												pos[1] + normal[1] * dist, 
												pos[2] + normal[2] * dist);
		sui_manipulator_line_draw(place, tmp, 0.01, 0.01, red, green, blue);
		sui_manipulator_point_draw(tmp, red, green, blue);
	}
	

	for(i = 0; i < 6; i++)
	{
		if(axis != i / 2)
		{
			if(input->mode == BAM_DRAW)
			{			
				sui_manipulator_vertex_draw(axis_pos[i], grab == id, red, green, blue);
			}else if(grab == id)
			{
				tmp[0] = axis_pos[i][0] - input->pointer_x;
				tmp[1] = axis_pos[i][1] - input->pointer_y;
				if(tmp[0] * tmp[0] + tmp[1] * tmp[1] < 0.01 * 0.01)
				{
					normal[0] = normal[1] = normal[2] = 0;
					normal[i / 2] = ((float)(i % 2) * 2.0 - 1.0);
				}
			}
		}
	}
	return grab == id;
}

boolean sui_manipulator_radius(BInputState *input, SViewData *v, float *pos, float *radius, void *id)
{
	static void *grab = NULL;
	float tmp[3], dist = 0.3, f, ring[4];
	uint i;
	sui_view_projection_screenf(v, tmp, pos[0], pos[1], pos[2]);
	dist = *radius / -tmp[2];
	tmp[0] -= input->pointer_x;
	tmp[1] -= input->pointer_y;
	if(input->mode == BAM_EVENT && grab == NULL && input->mouse_button[0] && !input->last_mouse_button[0])
	{

		if(tmp[0] * tmp[0] + tmp[1] * tmp[1] > (dist - 0.02) * (dist - 0.02) &&
			tmp[0] * tmp[0] + tmp[1] * tmp[1] < (dist + 0.02) * (dist + 0.02))
			grab = id;
	}
	if(input->mode == BAM_EVENT && grab == id)
		*radius = sqrt(tmp[0] * tmp[0] + tmp[1] * tmp[1]) * -tmp[2];
	if(!input->mouse_button[0])
		grab = NULL;
	printf("dist = %f\n", dist);
	if(input->mode == BAM_DRAW)
	{
		sui_view_projection_screenf(v, tmp, pos[0], pos[1], pos[2]);
		ring[0] = tmp[0];
		ring[1] = tmp[1] + dist;
		f = 0.2;
		if(grab == id)
			f = 0.8;
		for(i = 0; i < 64; )
		{
			i++;
			ring[2] = ring[0];
			ring[3] = ring[1];
			ring[0] = tmp[0] + sin((float)i * PI * 2.0 / 64.0) * dist;
			ring[1] = tmp[1] + cos((float)i * PI * 2.0 / 64.0) * dist;
			sui_draw_3d_line_gl(ring[0], ring[1], 0, ring[2], ring[3], 0, 1, 1, 1, f);
		}
	}
	return grab == id;
}

void sui_manipulator_normal_projection(SViewData *v, float *output, float *pos, float *normal, float pointer_x, float pointer_y)
{
	double camera[3];
	float aim[3], f, dist, angle;
	sui_view_camera_get(v, camera);
	sui_view_projectionf(v, aim, pointer_x, pointer_y);
	aim[0] -= camera[0];
	aim[1] -= camera[1];
	aim[2] -= camera[2];
	f = sqrt(aim[0] * aim[0] + aim[1] * aim[1] + aim[2] * aim[2]);
	aim[0] /= f;
	aim[1] /= f;
	aim[2] /= f;
	dist = (camera[0] - pos[0]) * normal[0] + (camera[1] - pos[1]) * normal[1] + (camera[2] - pos[2]) * normal[2];
	f = aim[0] * normal[0] + aim[1] * normal[1] + aim[2] * normal[2];
	output[0] = camera[0] + normal[0] * f;
	output[1] = camera[1] + normal[1] * f;
	output[2] = camera[2] + normal[2] * f;
/*	output[0] -= (aim[0] - normal[0] * f) / dist;
	output[1] -= (aim[1] - normal[1] * f) / dist;
	output[2] -= (aim[2] - normal[2] * f) / dist;*/
}

boolean sui_manipulator_matrix_projection(BInputState *input, SViewData *v, float *pos, float *normal, float *up, float *matrix, boolean scale, boolean aspect_lock, void *id)
{
	static void *grab = NULL;
	float tmp[3], tmp2[3], size_x, size_y, f, ring[4];
	double camera[3];
	uint i;
	if(scale)
		size_y = sqrt(up[0] * up[0] + up[1] * up[1] + up[2] * up[2]);
	else
		size_y = 1.0;
	if(!aspect_lock)
		size_x = sqrt(matrix[0] * matrix[0] + matrix[1] * matrix[1] + matrix[2] * matrix[2]);
	else
		size_x = size_y;

	f = up[0] * normal[0] + up[1] * normal[1] + up[2] * normal[2];
	up[0] -= normal[0] * f;
	up[1] -= normal[1] * f;
	up[2] -= normal[2] * f;
	f = sqrt(up[0] * up[0] + up[1] * up[1] + up[2] * up[2]);
	matrix[4] = up[0] = up[0] / f;
	matrix[5] = up[1] = up[1] / f;
	matrix[6] = up[2] = up[2] / f;
	matrix[7] = 0.0;
	
	matrix[8] = normal[0];
	matrix[9] = normal[1];
	matrix[10] = normal[2];
	matrix[11] = 0.0;

	matrix[0] = (matrix[9] * matrix[6] - matrix[10] * matrix[5]) * size_x;
	matrix[1] = (matrix[10] * matrix[4] - matrix[8] * matrix[6]) * size_x;
	matrix[2] = (matrix[8] * matrix[5] - matrix[9] * matrix[4]) * size_x;
	matrix[3] = 0.0;

	matrix[4] *= size_y;
	matrix[5] *= size_y;
	matrix[6] *= size_y;

	matrix[12] = pos[0];
	matrix[13] = pos[1];
	matrix[14] = pos[2];
	matrix[15] = 1.0;

	if(input->mode == BAM_DRAW)
	{
		glEnable(GL_LINE_SMOOTH);
		sui_view_projection_screenf(v, tmp, pos[0], pos[1], pos[2]);
		tmp[0] = 0;
		tmp[1] = 1;
		sui_view_projection_screenf(v, tmp, (matrix[0] * tmp[0]) + (matrix[4] * tmp[1]) + matrix[12],
											(matrix[1] * tmp[0]) + (matrix[5] * tmp[1]) + matrix[13],
											(matrix[2] * tmp[0]) + (matrix[6] * tmp[1]) + matrix[14]);
		ring[0] = tmp[0];
		ring[1] = tmp[1];
		f = 0.8;
		for(i = 0; i < 64; )
		{
			i++;
			ring[2] = ring[0];
			ring[3] = ring[1];
			tmp[0] = sin((float)i * PI * 2.0 / 64.0);
			tmp[1] = cos((float)i * PI * 2.0 / 64.0);
			sui_view_projection_screenf(v, tmp, (matrix[0] * tmp[0]) + (matrix[4] * tmp[1]) + matrix[12],
												(matrix[1] * tmp[0]) + (matrix[5] * tmp[1]) + matrix[13],
												(matrix[2] * tmp[0]) + (matrix[6] * tmp[1]) + matrix[14]);
			ring[0] = tmp[0];
			ring[1] = tmp[1];
			sui_draw_3d_line_gl(ring[0], ring[1], 0, ring[2], ring[3], 0, 1, 1, 1, f);
		}
		sui_view_projection_screenf(v, tmp, matrix[0] + matrix[4] + matrix[12],
											matrix[1] + matrix[5] + matrix[13],
											matrix[2] + matrix[6] + matrix[14]);
		sui_view_projection_screenf(v, tmp2, matrix[0] - matrix[4] + matrix[12],
											matrix[1] - matrix[5] + matrix[13],
											matrix[2] - matrix[6] + matrix[14]);
		sui_draw_3d_line_gl(tmp[0], tmp[1], 0, tmp2[0], tmp2[1], 0, 1, 1, 1, f);
		sui_view_projection_screenf(v, tmp, -matrix[0] - matrix[4] + matrix[12],
											-matrix[1] - matrix[5] + matrix[13],
											-matrix[2] - matrix[6] + matrix[14]);
		sui_draw_3d_line_gl(tmp[0], tmp[1], 0, tmp2[0], tmp2[1], 0, 1, 1, 1, f);
		sui_view_projection_screenf(v, tmp2, -matrix[0] + matrix[4] + matrix[12],
											-matrix[1] + matrix[5] + matrix[13],
											-matrix[2] + matrix[6] + matrix[14]);
		sui_draw_3d_line_gl(tmp[0], tmp[1], 0, tmp2[0], tmp2[1], 0, 1, 1, 1, f);
		sui_view_projection_screenf(v, tmp, matrix[0] + matrix[4] + matrix[12],
											matrix[1] + matrix[5] + matrix[13],
											matrix[2] + matrix[6] + matrix[14]);
		sui_draw_3d_line_gl(tmp[0], tmp[1], 0, tmp2[0], tmp2[1], 0, 1, 1, 1, f);
	}

/*	sui_manipulator_normal_projection(NULL, tmp, pos, &matrix[8], input->pointer_x, input->pointer_y);
	if(input->mouse_button[0])
	{
		up[0] = tmp[0] - matrix[12];
		up[1] = tmp[1] - matrix[13];
		up[2] = tmp[2] - matrix[14];
	}*/
	return grab == id;
}