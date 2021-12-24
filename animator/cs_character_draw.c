#include <math.h>

#include "st_matrix_operations.h"
#include "seduce.h"
#include "confuse.h"
#include "cs.h"
/*
extern CCharacter ca_character;
extern CCharDesc ca_description;

double sui_draw_slider(BInputState *input, double x_pos, double y_pos, double width, double value, float *start_color, float *end_color, float *color);
boolean ca_draw_settings_menu(BInputState *input, CCharDesc *c);
boolean ca_draw_menu_movement(BInputState *input, float *speed);
boolean ca_draw_anim_menu(BInputState *input, CCharDesc *c);
void ca_input_animator(BInputState *input, CCharDesc *d, CCharacter *c);
extern void ca_input_animator_edit_point_get(CCharDesc *desc, CCharacter *c, CAnimPoint *point, uint value_id);
extern void ca_input_animator_edit_point_set(CCharDesc *desc, CCharacter *c, CAnimPoint *point, uint value_id);
extern boolean ca_draw_menu_param(BInputState *input, CCharDesc *desc, CCharacter *c, uint selected);
*/

typedef struct{
	float min[3];
	float max[3];
}CABradBox;

CABradBox ca_draw_box_list[C_CM_COUNT];


void ca_draw_box_set(uint matrix, float min_x, float max_x, float min_y, float max_y, float min_z, float max_z)
{
	ca_draw_box_list[matrix].min[0] = min_x;
	ca_draw_box_list[matrix].min[1] = min_y;
	ca_draw_box_list[matrix].min[2] = min_z;
	ca_draw_box_list[matrix].max[0] = max_x;
	ca_draw_box_list[matrix].max[1] = max_y;
	ca_draw_box_list[matrix].max[2] = max_z;
}

void ca_draw_box_center_get(uint matrix, float *pos)
{
	pos[0] = (ca_draw_box_list[matrix].min[0] + ca_draw_box_list[matrix].max[0]) * 0.5;
	pos[1] = (ca_draw_box_list[matrix].min[1] + ca_draw_box_list[matrix].max[1]) * 0.5;
	pos[2] = (ca_draw_box_list[matrix].min[2] + ca_draw_box_list[matrix].max[2]) * 0.5;
}

uint ca_draw_box_test(float x, float y, float z, boolean arms, boolean legs, boolean tool, float expand)
{
	float tmp[3], vec[3], f, best = 0.04;
	uint i, found = -1;

	for(i = C_CM_BUTT; i < C_CM_COUNT; i++)
	{
		if((i < C_CM_ARM_LA_UP || i > C_CM_ARM_RB_DOWN || arms) &&
			(i !=C_CM_LEG_LA_UP || i > C_CM_LEG_RC_DOWN  || legs) &&
			(i != C_CM_TOOL || tool))
		{
			tmp[0] = x - ca_character.matrix[i][12];
			tmp[1] = y - ca_character.matrix[i][13];
			tmp[2] = z - ca_character.matrix[i][14];
			f = sqrt(ca_character.matrix[i][0] * ca_character.matrix[i][0] +
					ca_character.matrix[i][1] * ca_character.matrix[i][1] +
					 ca_character.matrix[i][2] * ca_character.matrix[i][2]);

			vec[0] = tmp[0] * ca_character.matrix[i][0] / f +
					tmp[1] * ca_character.matrix[i][1] / f +
					tmp[2] * ca_character.matrix[i][2] / f;

			vec[1] = tmp[0] * ca_character.matrix[i][4] / f +
					tmp[1] * ca_character.matrix[i][5] / f +
					tmp[2] * ca_character.matrix[i][6] / f;

			vec[2] = tmp[0] * ca_character.matrix[i][8] / f +
					tmp[1] * ca_character.matrix[i][9] / f +
					tmp[2] * ca_character.matrix[i][10] / f;
			if(vec[0] / f > ca_draw_box_list[i].min[0] - expand &&
				vec[0] / f  < ca_draw_box_list[i].max[0] + expand &&
				vec[1] / f  > ca_draw_box_list[i].min[1] - expand &&
				vec[1] / f  < ca_draw_box_list[i].max[1] + expand &&
				vec[2] / f  > ca_draw_box_list[i].min[2] - expand &&
				vec[2] / f  < ca_draw_box_list[i].max[2] + expand)
			{
				return i;
			}
		}
	}
	return C_CM_BASE;
}

void ca_draw_box_init()
{
	uint i;
	for(i = 0; i < C_CM_COUNT; i++)
		ca_draw_box_set(i, 0, 0, 0, 0, 0, 0);
	ca_draw_box_set(C_CM_BUTT, -1.0, 1.0, 0, -1, -0.2, 0.2);
	ca_draw_box_set(C_CM_BELLY, -0.4, 0.4, 0, 1, -0.2, 0.2);
	ca_draw_box_set(C_CM_TORSO, -0.4, 0.4, -ca_description.torso_size / ca_description.torso_width, 0, -0.2, 0.2);
	ca_draw_box_set(C_CM_HEAD, -0.4, 0.4, 0, 1, -0.6, 0.6);
	ca_draw_box_set(C_CM_LEG_LA_UP, -0.05, 0.05, -0.5, 0.0, -0.05, 0.05);
	ca_draw_box_set(C_CM_LEG_RA_UP, -0.05, 0.05, -0.5, 0.0, -0.05, 0.05);
	ca_draw_box_set(C_CM_LEG_LA_DOWN, -0.05, 0.05, -0.5, 0.0, -0.05, 0.05);
	ca_draw_box_set(C_CM_LEG_RA_DOWN, -0.05, 0.05, -0.5, 0.0, -0.05, 0.05);
	ca_draw_box_set(C_CM_ARM_LA_UP, -0.05, 0.05, -0.5, 0.0, -0.05, 0.05);
	ca_draw_box_set(C_CM_ARM_RA_UP, -0.05, 0.05, -0.5, 0.0, -0.05, 0.05);
	ca_draw_box_set(C_CM_ARM_LA_DOWN, -0.05, 0.05, -0.5, 0.0, -0.05, 0.05);
	ca_draw_box_set(C_CM_ARM_RA_DOWN, -0.05, 0.05, -0.5, 0.0, -0.05, 0.05);
	ca_draw_box_set(C_CM_ARM_RA_DOWN, -0.05, 0.05, -0.5, 0.0, -0.05, 0.05);
	ca_draw_box_set(C_CM_TOOL, -0.05, 0.05, -0.05, 0.05, 0.0, 0.5);
	ca_draw_box_set(C_CM_WORLD_MATRIX, -1, 1, -1, 1, -1, 1);
}

void ca_draw_box(float *matrix, float min_x, float max_x, float min_y, float max_y, float min_z, float max_z, float r, float g, float b)
{
	glPushMatrix();
	if(matrix != NULL)
		glMultMatrixf(matrix);
	sui_draw_3d_line_gl(min_x, min_y, min_z, max_x, min_y, min_z, r, g, b, 1);
	sui_draw_3d_line_gl(min_x, max_y, min_z, max_x, max_y, min_z, r, g, b, 1);
	sui_draw_3d_line_gl(min_x, min_y, max_z, max_x, min_y, max_z, r, g, b, 1);
	sui_draw_3d_line_gl(min_x, max_y, max_z, max_x, max_y, max_z, r, g, b, 1);

	sui_draw_3d_line_gl(min_x, min_y, min_z, min_x, max_y, min_z, r, g, b, 1);
	sui_draw_3d_line_gl(max_x, min_y, min_z, max_x, max_y, min_z, r, g, b, 1);
	sui_draw_3d_line_gl(min_x, min_y, max_z, min_x, max_y, max_z, r, g, b, 1);
	sui_draw_3d_line_gl(max_x, min_y, max_z, max_x, max_y, max_z, r, g, b, 1);

	sui_draw_3d_line_gl(min_x, min_y, min_z, min_x, min_y, max_z, r, g, b, 1);
	sui_draw_3d_line_gl(max_x, min_y, min_z, max_x, min_y, max_z, r, g, b, 1);
	sui_draw_3d_line_gl(min_x, max_y, min_z, min_x, max_y, max_z, r, g, b, 1);
	sui_draw_3d_line_gl(max_x, max_y, min_z, max_x, max_y, max_z, r, g, b, 1);
	glPopMatrix();
}

void ca_draw_box2(float *matrix, uint type, float r, float g, float b)
{
	glPushMatrix();
	glMultMatrixf(matrix);
	{
		float array[] = {-1, -1, -1, 
						-1, 1, -1, 
						1, 1, -1, 
						1, -1, -1,

						-1, 1, 1,
						-1, -1, 1, 
						1, -1, 1, 
						1, 1, 1, 

						-1, 1, -1,  
						-1, 1, 1,  
						1, 1, 1, 
						1, 1, -1,


						-1, -1, 1,
						-1, -1, -1, 						
						1, -1, -1,
						1, -1, 1, 

						
						-1, -1, -1, 
						-1, -1, 1,
						-1, 1, 1,
						-1, 1, -1,


						1, -1, 1,
						1, -1, -1, 						
						1, 1, -1,
						1, 1, 1
		};
		glTranslatef((ca_draw_box_list[type].max[0] + ca_draw_box_list[type].min[0]) * 0.5,
					(ca_draw_box_list[type].max[1] + ca_draw_box_list[type].min[1]) * 0.5,
					(ca_draw_box_list[type].max[2] + ca_draw_box_list[type].min[2]) * 0.5);
		glScalef((ca_draw_box_list[type].max[0] - ca_draw_box_list[type].min[0]) * 0.5,
				(ca_draw_box_list[type].max[1] - ca_draw_box_list[type].min[1]) * 0.5,
				(ca_draw_box_list[type].max[2] - ca_draw_box_list[type].min[2]) * 0.5);
		sui_draw_gl(GL_QUADS, array, 24, 3, r, g, b, 1);
	}
	glPopMatrix();
}

#define C_COLISSION_TEST_POINTS 10

void ca_ground_line(float x1, float y1, float x2, float y2)
{
	float f;
	f = 1 + ((x1 + x2) * (x1 + x2) + (y1 + y2) * (y1 + y2)) * 0.9;

	f = 0.2 + 1.0 / f;
	if(65 > (x1 + x2) * (x1 + x2) + (y1 + y2) * (y1 + y2))
		sui_draw_3d_line_gl(x1, 0, y1, x2, 0, y2, 0.1, 0.1, 0.1, 1.0);
}
void ca_ground_draw()
{
	float f, x, y;
	uint i, j;
	for(i = 0; i < 41; i++)
	{
		for(j = 0; j < 31; j++)
		{
			x = ((float)i / 20.0 - 1.0) * 4.0;
			y = ((float)j / 15.0 - 1.0) * 4.5; 



			ca_ground_line(x + 0.1, y,
								x + 0.1, y + 0.1);
			ca_ground_line(x + 0.1, y,
								x, y - 0.05);
			ca_ground_line(x + 0.1, y + 0.1,
								x, y + 0.15);
			ca_ground_line(x - 0.1, y,
								x, y - 0.05);
			ca_ground_line(x - 0.1, y + 0.1,
								x, y + 0.15);
			ca_ground_line(x, y + 0.15,
								x, y + 0.25);

		}
	}
}

void ca_character_draw(float *tool_matrix, uint tool_count)
{
	float size = 0.9, r = 0.8, g = 0.3, b = 0.6, f;
	uint i, j, k, matrix;
	ca_draw_box_init();
	for(i = 0; i < tool_count; i++)
		ca_draw_box2(&tool_matrix[i * 16], C_CM_TOOL, 0.25, 0.25, 0.25);
	for(i = 0; i < C_CM_COUNT; i++)
//		if(i != C_CM_TOOL)
			ca_draw_box2(ca_character.matrix[i], i, 0.25, 0.25, 0.25);
	glEnable(GL_CULL_FACE);
	glPolygonOffset(-3.0, -3.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glDepthMask(FALSE);
	glPolygonOffset(3.0, 3.0);
	glDepthFunc(GL_GREATER);
	for(i = 0; i < tool_count; i++)
		ca_draw_box2(&tool_matrix[i * 16], C_CM_TOOL, 0.15, 0.15, 0.15);
	for(i = 0; i < C_CM_COUNT; i++)
//		if(i != C_CM_TOOL)
			ca_draw_box2(ca_character.matrix[i], i, 0.15, 0.15, 0.15);
	glPolygonOffset(-1.0, 2.0);
	glDepthFunc(GL_LEQUAL);
	for(i = 0; i < tool_count; i++)
		ca_draw_box2(&tool_matrix[i * 16], C_CM_TOOL, 1.3, 0.8, 0.5);
	for(i = 0; i < C_CM_COUNT; i++)
//		if(i != C_CM_TOOL)
			ca_draw_box2(ca_character.matrix[i], i, 0.3, 0.8, 0.5);

	glDisable(GL_POLYGON_OFFSET_LINE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);
	glDepthMask(TRUE);

	if(ca_character_stroke_length > 1)
	{
		sui_draw_gl(GL_LINES, ca_character_stroke, ca_character_stroke_length, 3, 0.25, 0.5, 0.45, 0.1);
		sui_draw_gl(GL_LINES, &ca_character_stroke[3], ca_character_stroke_length - 1, 3, 0.25, 0.5, 0.45, 0.1);

		sui_draw_gl(GL_POINTS, ca_character_stroke, 1, 3, 1, 1, 1, 0.1);
		sui_draw_gl(GL_POINTS, &ca_character_stroke[(ca_character_stroke_length - 1) * 3], 1, 3, 1, 1, 1, 0.1);
	}

}