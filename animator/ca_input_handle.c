#include <math.h>

#include "st_matrix_operations.h"
#include "seduce.h"
#include "confuse.h"
#include "cs.h"

uint ca_draw_box_test(float x, float y, float z, boolean arms, boolean legs, boolean tool, float expand);
void ca_draw_box_center_get(uint matrix, float *pos);

CCharacter ca_character;
CCharDesc ca_description;

extern float ca_animator_walk_speed;

uint ca_animator_selected_clip = -1;
uint ca_animator_selected_point = 0;
uint ca_animator_selected_key = -1;
uint ca_animator_selected_space = -1;
uint ca_animator_selected_tool = -1;
float ca_animator_selected_time = 0;
boolean ca_animator_play = FALSE;


uint ca_character_find_line(float *array, uint length, float dist)
{
	float sum[3], vec[3], tmp[3];
	uint i, j;
	for(i = 2; i < length - 1; i++)
	{
		sum[0] = array[0];
		sum[1] = array[1];
		sum[2] = array[2];
		vec[0] = (array[i * 3 + 0] - sum[0]) / (float)i;
		vec[1] = (array[i * 3 + 1] - sum[1]) / (float)i;
		vec[2] = (array[i * 3 + 2] - sum[2]) / (float)i;
		for(j = 1; j < i; j++)
		{
			sum[0] += vec[0];
			sum[1] += vec[1];
			sum[2] += vec[2];
			tmp[0] = array[j * 3 + 0] - sum[0];
			tmp[1] = array[j * 3 + 1] - sum[1];
			tmp[2] = array[j * 3 + 2] - sum[2];
			if(tmp[0] * tmp[0] + tmp[1] * tmp[1] + tmp[2] * tmp[2] > dist * dist)
				return i - 1;
		}
	}
	return length - 1;
}

void ca_point_key_frame_recording(float *stroke, uint length, uint value_id)
{
	CAnimPoint point, p;
	uint i = 0;
	point.pos[0] = stroke[0];
	point.pos[1] = stroke[1];
	point.pos[2] = stroke[2];
	p.time = point.time = 0;
	p.space = point.space = C_CM_BASE;
	c_character_to_matrix(p.pos, &ca_character, &point, value_id);
	ca_input_animator_edit_point_set(&ca_description, &ca_character, &p, value_id);

	while(i < length - 2)
	{
		i += ca_character_find_line(&stroke[i * 3], length - i, 0.03);
		point.pos[0] = stroke[i * 3];
		point.pos[1] = stroke[i * 3 + 1];
		point.pos[2] = stroke[i * 3 + 2];
		p.time = point.time = (float)i / (float)(length - 1);
		p.space = point.space = C_CM_BASE;
		c_character_to_matrix(p.pos, &ca_character, &point, value_id);
		ca_input_animator_edit_point_set(&ca_description, &ca_character, &p, value_id);
	}
}

boolean ca_point_draw(BInputState *input, CAnimPoint *point, float scale, uint value_id, float color)
{
	float pos[3];
	c_character_from_matrix(pos, &ca_character, point, value_id);
	sui_view_projection_screenf(NULL, pos, pos[0], pos[1], pos[2]);
	pos[2] = -1;
	if(input->mode == BAM_DRAW)
		sui_manipulator_point_draw(pos[0], pos[1], scale, color, color, color);
	else if(input->mouse_button[0] && !input->last_mouse_button[0])
		if(scale * scale * 0.01 * 0.01 > (input->pointer_x - pos[0]) * (input->pointer_x - pos[0]) + (input->pointer_y - pos[1]) * (input->pointer_y - pos[1]))
			return TRUE;
	return FALSE; 
}




boolean ca_point_edit(BInputState *input, CAnimPoint *point, uint value_id, float color, void *id)
{
	static uint grab = -1;
	static float timer = 0;
	float pos[3], tmp[3], normal[3];
	CAnimPoint p;
	uint space;
	boolean select = FALSE;
	c_character_from_matrix(p.pos, &ca_character, point, value_id);

	if(!input->mouse_button[0] && grab == value_id)
	{
		ca_character_stroke_rec = FALSE;
		ca_point_key_frame_recording(ca_character_stroke, ca_character_stroke_length, value_id);
		grab = -1;

	}	

	if(sui_manipulator_point(input, NULL, p.pos, id, color, color, color, c_point_name[value_id]))
	{
		select = TRUE;
		if(grab == -1 && ca_character_stroke_rec)
		{
			ca_character_stroke_length = 0;
			timer = 0.025;
			grab = value_id;
			
		}
	}
	p.space = point->space;
	p.time = ca_animator_selected_time;//point->time;

	if(input->mode == BAM_EVENT && grab == value_id)
	{
		timer += betray_get_delta_time();
		if(timer > 0.025)
		{
			timer -= 0.025;
			if(ca_character_stroke_length % 1024 == 0)
				ca_character_stroke = realloc(ca_character_stroke, (sizeof *ca_character_stroke) * (ca_character_stroke_length + 1024) * 3);
			ca_character_stroke[ca_character_stroke_length * 3 + 0] = p.pos[0];
			ca_character_stroke[ca_character_stroke_length * 3 + 1] = p.pos[1];
			ca_character_stroke[ca_character_stroke_length * 3 + 2] = p.pos[2];
			ca_character_stroke_length++;
			printf("ca_character_stroke_length[%u] %u - %f %f %f\n", value_id, ca_character_stroke_length, p.pos[0], p.pos[1], p.pos[2]);
		}
	}

	c_character_to_matrix(point->pos, &ca_character, &p, value_id);

	if(select)
	{
		if(value_id == C_APC_HAND_LEFT ||
			value_id == C_APC_HAND_RIGHT ||
			value_id == C_APC_ARM_LEFT ||
			value_id == C_APC_ARM_RIGHT ||
			value_id == C_APC_HEAD_LOOK ||
			value_id == C_APC_TOOL_POS ||
			value_id == C_APC_TOOL_DIR ||
			value_id == C_APC_TOOL_UP)
		{
			space = ca_draw_box_test(p.pos[0], p.pos[1], p.pos[2], 
			/*	value_id == C_APC_TOOL_POS ||
				value_id == C_APC_TOOL_DIR ||
				value_id == C_APC_TOOL_UP*/
				value_id == C_APC_HEAD_LOOK,
				TRUE, 
				value_id != C_APC_TOOL_POS &&
				value_id != C_APC_TOOL_DIR &&
				value_id != C_APC_TOOL_UP, 0.1);
			if(point->space != space)
			{
				float tmp[3];
				c_character_from_matrix(tmp, &ca_character, point, value_id);
				point->pos[0] = tmp[0];
				point->pos[1] = tmp[1];
				point->pos[2] = tmp[2];
				point->space = space;
				c_character_to_matrix(tmp, &ca_character, point, value_id);
				point->pos[0] = tmp[0];
				point->pos[1] = tmp[1];
				point->pos[2] = tmp[2];
			}
		}
	}
	return select;
}


void ca_point_move(BInputState *input, CAnimPoint *point, uint value_id)
{

	static uint grab = -1;
	static float timer = 0;
	float pos[3], tmp[3], normal[3];
	CAnimPoint p;
	uint space;
	boolean select = FALSE;
	c_character_from_matrix(p.pos, &ca_character, point, value_id);
	sui_view_projection_screenf(NULL, tmp, p.pos[0], p.pos[1], p.pos[2]);

	tmp[0] += input->delta_pointer_x;
	tmp[1] += input->delta_pointer_y;

	sui_view_projection_vertexf(NULL, p.pos, p.pos, tmp[0], tmp[1]);

	p.space = point->space;
	p.time = ca_animator_selected_time;//point->time;
	c_character_to_matrix(point->pos, &ca_character, &p, value_id);
}

void ca_point_space(CAnimPoint *point, uint value_id)
{
	float pos[3];
	uint space;
	c_character_from_matrix(pos, &ca_character, point, value_id);
	if(value_id == C_APC_HAND_LEFT ||
		value_id == C_APC_HAND_RIGHT ||
		value_id == C_APC_ARM_LEFT ||
		value_id == C_APC_ARM_RIGHT ||
		value_id == C_APC_HEAD_LOOK ||
		value_id == C_APC_TOOL_POS ||
		value_id == C_APC_TOOL_DIR ||
		value_id == C_APC_TOOL_UP)
	{
		space = ca_draw_box_test(pos[0], pos[1], pos[2], 
		/*	value_id == C_APC_TOOL_POS ||
			value_id == C_APC_TOOL_DIR ||
			value_id == C_APC_TOOL_UP*/ FALSE,
			TRUE, 
			value_id != C_APC_TOOL_POS &&
			value_id != C_APC_TOOL_DIR &&
			value_id != C_APC_TOOL_UP, 0.1);
	}else
	{
		space = C_APC_BASE;
		printf("Aspace %u %u\n", point->space, space);
	}
	if(point->space != space/* && point->space == -1*/)
	{
		float tmp[3];
		c_character_from_matrix(tmp, &ca_character, point, value_id);
		point->pos[0] = tmp[0];
		point->pos[1] = tmp[1];
		point->pos[2] = tmp[2];
		point->space = space;
		c_character_to_matrix(tmp, &ca_character, point, value_id);
		point->pos[0] = tmp[0];
		point->pos[1] = tmp[1];
		point->pos[2] = tmp[2];
	}

}

void ca_point_space_old(CAnimPoint *point, uint value_id)
{
	float pos[3];
	uint space;
	c_character_from_matrix(pos, &ca_character, point, value_id);
	if(value_id == C_APC_HAND_LEFT ||
		value_id == C_APC_HAND_RIGHT ||
		value_id == C_APC_ARM_LEFT ||
		value_id == C_APC_ARM_RIGHT ||
		value_id == C_APC_HEAD_LOOK ||
		value_id == C_APC_TOOL_POS ||
		value_id == C_APC_TOOL_DIR ||
		value_id == C_APC_TOOL_UP)
	{
		space = ca_draw_box_test(pos[0], pos[1], pos[2], 
		/*	value_id == C_APC_TOOL_POS ||
			value_id == C_APC_TOOL_DIR ||
			value_id == C_APC_TOOL_UP*/ FALSE,
			TRUE, 
			value_id != C_APC_TOOL_POS &&
			value_id != C_APC_TOOL_DIR &&
			value_id != C_APC_TOOL_UP, 0.1);
		if(point->space != space)
		{
			float tmp[3];
			c_character_from_matrix(tmp, &ca_character, point, value_id);
			point->pos[0] = tmp[0];
			point->pos[1] = tmp[1];
			point->pos[2] = tmp[2];
			point->space = space;
			c_character_to_matrix(tmp, &ca_character, point, value_id);
			point->pos[0] = tmp[0];
			point->pos[1] = tmp[1];
			point->pos[2] = tmp[2];
		}
	}
}

void ca_space_handle_pos_get(uint matrix, uint clip, uint id, uint key, float *x, float *y)
{
	CAnimPoint point;
	float pos[3], tmp[3];
	if(matrix == C_CM_LOOK)
	{
		sui_view_projection_screenf(NULL, tmp, ca_character.matrix[C_CM_HEAD][12], ca_character.matrix[C_CM_HEAD][13], ca_character.matrix[C_CM_HEAD][14]);
		tmp[0] = ca_character.matrix[C_CM_HEAD][12] + ca_character.look_dir[0] * -tmp[2] * 0.3;
		tmp[1] = ca_character.matrix[C_CM_HEAD][13] + ca_character.look_dir[1] * -tmp[2] * 0.3;
		tmp[2] = ca_character.matrix[C_CM_HEAD][14] + ca_character.look_dir[2] * -tmp[2] * 0.3;
		sui_view_projection_screenf(NULL, pos, tmp[0], tmp[1], tmp[2]);
		pos[1] -= 0.03;
	}else if(matrix == C_CM_BASE)
	{
		if(key != -1 && clip != -1)
			point = ca_description.events[clip].anim[id].points[key];
		else
			ca_input_animator_edit_point_get(&ca_description, &ca_character, &point, id);

		c_character_from_matrix(pos, &ca_character, &point, id);
		sui_view_projection_screenf(NULL, pos, pos[0], pos[1], pos[2]);
		pos[1] -= 0.03;
	}else
	{
		point.space = matrix;
		ca_draw_box_center_get(point.space, point.pos);
		c_character_from_matrix(pos, &ca_character, &point, id);
		sui_view_projection_screenf(NULL, pos, pos[0], pos[1], pos[2]);
	}
	*x = pos[0];
	*y = pos[1];
}


typedef enum{
	CA_IM_IDLE,
	CA_IM_MOVE,
	CA_IM_SPACE,
	CA_IM_VIEW,
	CA_IM_POPUP,
	CA_IM_MENU,
	CA_IM_MANIPULATOR
}CAInputMode;

void ca_input_handler(BInputState *input, void *user_pointer)
{
	static CAInputMode mode = CA_IM_IDLE;
	static boolean init = FALSE, click_lock = FALSE;
	static float hands[2][3] = {0.02, 0.04, 0.02, 0.02, 0.04, -0.02}, look_dir[3] = {1, 0, 0};
	static float world_matrix[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0.0, 1.5, 0.2, 0, 1}, matrix_size = 0.2, timer = 0;
	CAnimPoint point, *p;
	uint i, j;

	if(input->mode == BAM_MAIN)
	{
		return;
	}
	if(!init)
	{
		float pos[3] = {0, 0, 0};
		c_character_init(&ca_character, pos);
		c_character_create_humanoid(&ca_description, 0.015, 45, 0.02);
		c_character_load(&ca_description, 1.0);
		init = TRUE;
	}

	if(input->mode == BAM_DRAW)
	{
		glClearColor(0.2, 0.2, 0.2, 0);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glPushMatrix();
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		sui_view_set(NULL);
		{
			static float pos[3] = {0, 0, 0}, upp[3] = {0, 1, 0}, timer = 0, size = 0.01;
			float tool_matrix[128 * 4];
			uint tools[128];

			timer += betray_get_delta_time() * 1;
			pos[0] = sin(timer) * ca_animator_walk_speed * 9.0;
			pos[1] = ca_description.legs_size;
			pos[2] = cos(timer) * ca_animator_walk_speed * 9.0;

			upp[0] = 0;
			upp[1] = 1;
			upp[2] = 0;

			for(i = 0; i < ca_description.event_count; i++)
				tools[i] = i;
			c_character_animate(&ca_character, &ca_description, pos, upp, look_dir, ca_description.event_count, tools, tool_matrix, ca_animator_selected_tool, betray_get_delta_time());	
			ca_ground_draw();
			ca_character_draw(tool_matrix, ca_description.event_count);

		}
		for(i = 0; i < C_APC_COUNT; i++)
		{
			float line[6], pos[3], f, dist;
			if(ca_animator_selected_clip < ca_description.event_count)
			{
				if(ca_description.events[ca_animator_selected_clip].anim[i].point_count > 0)
				{
					for(j = 0; j < ca_description.events[ca_animator_selected_clip].anim[i].point_count - 1 || ( 
						ca_description.events[ca_animator_selected_clip].loop &&
						j < ca_description.events[ca_animator_selected_clip].anim[i].point_count); j++)
					{
						c_character_from_matrix(&line[0], &ca_character, &ca_description.events[ca_animator_selected_clip].anim[i].points[j], i);
						c_character_from_matrix(&line[3], &ca_character, &ca_description.events[ca_animator_selected_clip].anim[i].points[(j + 1) % ca_description.events[ca_animator_selected_clip].anim[i].point_count], i);
						
						if(j < ca_description.events[ca_animator_selected_clip].anim[i].point_count - 1)
							f = ca_description.events[ca_animator_selected_clip].anim[i].points[j].time;
						else
							f = 0.1;
						
						if(ca_animator_selected_point != i)
							sui_draw_3d_line_gl(line[0], 
												line[1], 
												line[2], 
												line[3], 
												line[4], 
												line[5], 0.3, 0.3, 0.3, 1);
						else for(dist = 0; dist + 0.005 / f < 1.0; dist += 0.01 / f)
							sui_draw_3d_line_gl(line[0] * (dist) + line[3] * (1 - dist), 
												line[1] * (dist) + line[4] * (1 - dist), 
												line[2] * (dist) + line[5] * (1 - dist), 
												line[0] * (dist + 0.005 / f) + line[3] * (1 - dist - 0.005 / f), 
												line[1] * (dist + 0.005 / f) + line[4] * (1 - dist - 0.005 / f), 
												line[2] * (dist + 0.005 / f) + line[5] * (1 - dist - 0.005 / f), 0.8, 0.8, 0.8, 1);
					}
				}
			}
		}
		glPopMatrix();
		glPushMatrix();
		glTranslatef(0, 0, -1);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/*	{
			char *mode_names[] = {"CA_IM_IDLE", "CA_IM_MOVE", "CA_IM_SPACE", "CA_IM_VIEW", "CA_IM_POPUP", "CA_IM_MENU", "CA_IM_MANIPULATOR"};
			char nr[32];
			sprintf(nr, "clip %u", ca_animator_selected_clip);
			sui_draw_text(0.5, 0.44, SUI_T_SIZE, SUI_T_SPACE, nr, 1, 0, 0, 0.5);
			sprintf(nr, "point %u", ca_animator_selected_point);
			sui_draw_text(0.5, 0.42, SUI_T_SIZE, SUI_T_SPACE, nr, 1, 0, 0, 0.5);
			sprintf(nr, "space %u", ca_animator_selected_space);
			sui_draw_text(0.5, 0.4, SUI_T_SIZE, SUI_T_SPACE, nr, 1, 0, 0, 0.5);
			sprintf(nr, "key %u", ca_animator_selected_key);
			sui_draw_text(0.5, 0.38, SUI_T_SIZE, SUI_T_SPACE, nr, 1, 0, 0, 0.5);

			if(ca_animator_selected_clip != -1 && ca_animator_selected_point != -1)
			{
				if(ca_animator_selected_key != -1)
					point = ca_description.events[ca_animator_selected_clip].anim[ca_animator_selected_point].points[ca_animator_selected_key];
				else
					point = ca_description.default_pose[ca_animator_selected_point];
				//	ca_input_animator_edit_point_get(&ca_description, &ca_character, &point, ca_animator_selected_point);

				sprintf(nr, "Current space %u %f", point.space, point.time);
				sui_draw_text(0.5, 0.36, SUI_T_SIZE, SUI_T_SPACE, nr, 1, 0, 0, 0.5);
			}

			sui_draw_text(0.5, 0.34, SUI_T_SIZE, SUI_T_SPACE, mode_names[mode], 1, 0, 0, 0.5);
		//	printf("%s\n", mode_names[mode]);
		}*/
	}

	/**/

	if(ca_draw_settings_menu(input, &ca_description))
		mode = CA_IM_MENU;
	ca_input_animator(input, &ca_description, &ca_character);
	if(mode == CA_IM_IDLE && input->mouse_button[0] && !input->last_mouse_button[0] &&
		input->pointer_y < 0.16 - betray_get_screen_mode(NULL, NULL, NULL) && ca_animator_selected_clip != -1)
			mode = CA_IM_MENU;


	if(mode == CA_IM_IDLE || mode == CA_IM_MANIPULATOR || input->mode == BAM_DRAW)
	{
		uint i;
		float m[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};


		if(sui_manipulator_normal(input, NULL, &ca_character.matrix[C_CM_HEAD][12], look_dir, look_dir, 1, 1, 1))
		{
			ca_animator_selected_point = -1;
			mode = CA_IM_MANIPULATOR;
		}

		if(sui_manipulator_point(input, NULL, &world_matrix[12], &world_matrix[12], 1, 1, 1, "MATRIX"))
			mode = CA_IM_MANIPULATOR;

		world_matrix[8] /= matrix_size;
		world_matrix[9] /= matrix_size;
		world_matrix[10] /= matrix_size;
		if(sui_manipulator_normal(input, NULL, &world_matrix[12], &world_matrix[8], &world_matrix[8], 1, 1, 1))
			mode = CA_IM_MANIPULATOR;
		world_matrix[4] /= matrix_size;
		world_matrix[5] /= matrix_size;
		world_matrix[6] /= matrix_size;
		if(sui_manipulator_normal(input, NULL, &world_matrix[12], &world_matrix[4], &world_matrix[4], 0.5, 0.5, 0.5))
			mode = CA_IM_MANIPULATOR;

		matrix_size *= sqrt(3.0);
		if(sui_manipulator_radius(input, NULL, &world_matrix[12], &matrix_size, &matrix_size))
			mode = CA_IM_MANIPULATOR;
		matrix_size /= sqrt(3.0);
		c_character_matrix(m, &world_matrix[4], &world_matrix[8], &world_matrix[12]);
		for(i = 0; i < 12; i++)
			world_matrix[i] = m[i] * matrix_size;
		c_character_world_matrix_set(&ca_character, world_matrix);
	}
	{
		float color[4] = {1, 1, 1, 1}, pos[3], tmp[3], normal[3], f, line[6];
		CAnimPoint point, p;
		boolean found = FALSE;
		for(i = 0; i < C_APC_COUNT; i++)
		{
			if(i == ca_animator_selected_point)
				f = 0.8;
			else
			{
				if(ca_animator_selected_clip < ca_description.event_count &&
					ca_description.events[ca_animator_selected_clip].anim[i].point_count == 0)
					f = 0.2;
				else
					f = 0.4;
			}
			if(ca_animator_selected_clip < ca_description.event_count && ca_animator_selected_point == i)
			{
				if(ca_character_stroke_rec)
					ca_animator_selected_key = -1;
				else
				{
					for(j = 0; j < ca_description.events[ca_animator_selected_clip].anim[i].point_count; j++)
					{
						point = ca_description.events[ca_animator_selected_clip].anim[i].points[j];
						if(ca_point_draw(input, &point, 0.5, i, f))
						{
							ca_animator_selected_point = i;
							ca_animator_selected_key = j;
							found = TRUE;
						}
					}
				}
			}

			ca_input_animator_edit_point_get(&ca_description, &ca_character, &point, i);
			if(ca_point_draw(input, &point, 1.0, i, f) && mode == CA_IM_IDLE && input->mode == BAM_EVENT)
			{
				ca_animator_selected_point = i;
				point.time = ca_animator_selected_time;
				ca_animator_selected_key = ca_input_animator_edit_point_set(&ca_description, &ca_character, &point, i);
			}
		}
	}



	/* get selected point / key */

	if(ca_animator_selected_point != -1)
	{
		if(ca_animator_selected_clip != -1 && ca_animator_selected_key != -1)
			point = ca_description.events[ca_animator_selected_clip].anim[ca_animator_selected_point].points[ca_animator_selected_key];
		else
			ca_input_animator_edit_point_get(&ca_description, &ca_character, &point, ca_animator_selected_point);
	}

	/* draw space connectors */

	if(ca_animator_selected_point != -1)
	{
		float tmp[3], pos[2];
		ca_space_handle_pos_get(point.space, ca_animator_selected_clip, ca_animator_selected_point, ca_animator_selected_key, &pos[0], &pos[1]);
		sui_manipulator_point_draw(pos[0], pos[1], 0.7, 0.8, 0.8, 0.8);

		c_character_from_matrix(tmp, &ca_character, &point, ca_animator_selected_point);
		sui_view_projection_screenf(NULL, tmp, tmp[0], tmp[1], tmp[2]);

		sui_draw_3d_line_gl(pos[0] * 0.8 + tmp[0] * 0.2,
							pos[1] * 0.8 + tmp[1] * 0.2, 0,
							pos[0] * 0.2 + tmp[0] * 0.8, 
							pos[1] * 0.2 + tmp[1] * 0.8, 0, 0.7, 0.7, 0.7, 1);


	/*	if(ca_animator_selected_clip != 0)
		{
			for(i = 0; i < ca_description.events[ca_animator_selected_clip].anim[ca_animator_selected_point].point_count; i++)
			{
				CAnimPoint po;
				po = ca_description.events[ca_animator_selected_clip].anim[ca_animator_selected_point].points[i];

				ca_space_handle_pos_get(po.space, ca_animator_selected_clip, ca_animator_selected_point, i, &pos[0], &pos[1]);
				sui_manipulator_point_draw(pos[0], pos[1], 0.7, 0.8, 0.8, 0.8);

				c_character_from_matrix(tmp, &ca_character, &po, ca_animator_selected_point);
				sui_view_projection_screenf(NULL, tmp, tmp[0], tmp[1], tmp[2]);

				sui_draw_3d_line_gl(pos[0] * 0.8 + tmp[0] * 0.2,
									pos[1] * 0.8 + tmp[1] * 0.2, 0,
									pos[0] * 0.2 + tmp[0] * 0.8, 
									pos[1] * 0.2 + tmp[1] * 0.8, 0, 0.4, 0.4, 0.4, 1);	
			}
		}*/
	}


	/* grab on to SPACE and POINTS */

	if(input->mode == BAM_EVENT && mode == CA_IM_IDLE && ca_animator_selected_point != -1)
	{
		if(input->mouse_button[0] && !input->last_mouse_button[0])
		{
			float pos[2];
			ca_space_handle_pos_get(point.space, ca_animator_selected_clip, ca_animator_selected_point, ca_animator_selected_key, &pos[0], &pos[1]);
			pos[0] -= input->pointer_x;
			pos[1] -= input->pointer_y;
			if(0.01 * 0.01 > pos[0] * pos[0] + pos[1] * pos[1])
			{
				ca_animator_selected_space = i;
				mode = CA_IM_SPACE;
			}else
				mode = CA_IM_MOVE;
		}
	} 

	/* MOVE Points and Keys*/

	if(input->mode == BAM_EVENT && mode == CA_IM_MOVE)
	{
		if(ca_animator_selected_point != -1)
		{
			if(ca_animator_selected_clip == -1)
				ca_animator_selected_key = -1;

			if(ca_animator_selected_key != -1)
			{
				ca_point_move(input, &ca_description.events[ca_animator_selected_clip].anim[ca_animator_selected_point].points[ca_animator_selected_key], ca_animator_selected_point);
				if(ca_description.events[ca_animator_selected_clip].anim[ca_animator_selected_point].points[ca_animator_selected_key].space == C_CM_BASE)
					ca_point_space(&ca_description.events[ca_animator_selected_clip].anim[ca_animator_selected_point].points[ca_animator_selected_key], ca_animator_selected_point);
			}
			else
			{
				CAnimPoint point;
				ca_input_animator_edit_point_get(&ca_description, &ca_character, &point, ca_animator_selected_point);
				ca_point_move(input, &point, ca_animator_selected_point);

				if(ca_character_stroke_rec)
				{
					timer += betray_get_delta_time();
					ca_animator_selected_time += betray_get_delta_time() / ca_description.events[ca_animator_selected_clip].speed;
					if(ca_animator_selected_time > 1.0)
						ca_animator_selected_time = 1.0;
					
					ca_description.events[ca_animator_selected_clip].anim[ca_animator_selected_point].point_count = 0;
					if(timer > 0.025)
					{
						timer -= 0.025;
						if(ca_character_stroke_length % 1024 == 0)
							ca_character_stroke = realloc(ca_character_stroke, (sizeof *ca_character_stroke) * (ca_character_stroke_length + 1024) * 3);
						c_character_from_matrix(&ca_character_stroke[ca_character_stroke_length * 3 + 0], &ca_character, &point, ca_animator_selected_point);
					/*	ca_character_stroke[ca_character_stroke_length * 3 + 0] = point.pos[0];
						ca_character_stroke[ca_character_stroke_length * 3 + 1] = point.pos[1];
						ca_character_stroke[ca_character_stroke_length * 3 + 2] = point.pos[2];*/
						ca_character_stroke_length++;
					}
				}
				ca_point_space(&point, ca_animator_selected_point);
				ca_input_animator_edit_point_set(&ca_description, &ca_character, &point, ca_animator_selected_point);
			}
		}
	}

	if(input->mode == BAM_EVENT && mode == CA_IM_SPACE)
	{
		float pos[2], tmp[3], best = 100000.0, f;
		i = C_CM_BASE;
		for(j = 0; j < C_CM_COUNT; j++)
		{
			ca_space_handle_pos_get(j, ca_animator_selected_clip, ca_animator_selected_point, ca_animator_selected_key, &pos[0], &pos[1]);
			pos[0] -= input->pointer_x;
			pos[1] -= input->pointer_y;
			f = pos[0] * pos[0] + pos[1] * pos[1];
			if(f < best)
			{
				best = f;
				i = j;
			}
		}
		if(point.space != i)
		{
			if(point.space != -1)
			{
				c_character_from_matrix(tmp, &ca_character, &point, ca_animator_selected_point);
				point.pos[0] = tmp[0];
				point.pos[1] = tmp[1];
				point.pos[2] = tmp[2];
			}
			point.space = i;
			c_character_to_matrix(tmp, &ca_character, &point, ca_animator_selected_point);
			point.pos[0] = tmp[0];
			point.pos[1] = tmp[1];
			point.pos[2] = tmp[2];
			printf("setting key %u %u\n", point.space, i);
			if(ca_animator_selected_key != -1)
			{
				printf("A setting key %u %u\n", point.space, i);
				ca_description.events[ca_animator_selected_clip].anim[ca_animator_selected_point].points[ca_animator_selected_key] = point;
			}else
			{
				printf("B setting key %u %u\n", point.space, i);
				ca_input_animator_edit_point_set(&ca_description, &ca_character, &point, ca_animator_selected_point);
			}
		}
	}

	/* end recording */

	if(input->mode == BAM_EVENT && !input->mouse_button[0] && !input->last_mouse_button[0] && mode == CA_IM_MOVE && ca_character_stroke_rec)
	{
		timer = 0;
		ca_description.events[ca_animator_selected_clip].anim[ca_animator_selected_point].point_count = 0;
		ca_point_key_frame_recording(ca_character_stroke, ca_character_stroke_length, ca_animator_selected_point);
		ca_character_stroke_length = 0;
		ca_character_stroke_rec = FALSE;
	}		

	if(mode == CA_IM_IDLE && input->mouse_button[2])
		mode = CA_IM_POPUP;
	if(mode == CA_IM_MENU || (!input->mouse_button[0] && !input->mouse_button[1] && !input->mouse_button[2]) || input->mode == BAM_DRAW)
		ca_draw_timeline(input, &ca_description, &ca_character, betray_get_delta_time());

	if(mode == CA_IM_IDLE && input->mouse_button[2])
		mode = CA_IM_POPUP;
	if(mode == CA_IM_POPUP)
		ca_draw_popup(&ca_description, &ca_character, input);

	if(mode == CA_IM_IDLE && input->mouse_button[1])
		mode = CA_IM_VIEW;
	if(mode == CA_IM_VIEW && input->mode == BAM_EVENT)
		sui_view_change(NULL, input);

	if(input->mode == BAM_DRAW)
	{
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glPopMatrix();
	}
	if(input->mode == BAM_EVENT &&
		!input->mouse_button[0] && !input->last_mouse_button[0] &&
		!input->mouse_button[1] && !input->last_mouse_button[1] &&
		!input->mouse_button[2] && !input->last_mouse_button[2])
			mode = CA_IM_IDLE;	
}