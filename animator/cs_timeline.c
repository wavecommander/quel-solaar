#include <math.h>

#include "st_matrix_operations.h"
#include "seduce.h"
#include "confuse.h"
#include "cs.h"
/*
extern uint ca_animator_selected_clip;
extern uint ca_animator_selected_point;
extern uint ca_animator_selected_key;
float ca_animator_selected_time = 0.0;
boolean ca_animator_play = FALSE;
*/
void sui_draw_window(double x_pos, double y_pos, double width, double height, float radius, float red, float green, float blue, float alpha, char *title);

boolean ca_character_stroke_rec = FALSE;
float *ca_character_stroke = NULL;
uint ca_character_stroke_length = 0;


void ca_draw_key_frame(float pos_x, float pos_y, boolean select)
{
	static float shadow[2 * 4 * 5], color[4 * 4 * 5], polys[6 * 3] = {0.0, 0.0, -0.005, 0.005, 0.005, 0.005, -0.005, 0.005, -0.005, 0.02, 0.005, 0.02, -0.005, 0.005, 0.005, 0.005, 0.005, 0.02}, f;
	static boolean init = TRUE;
	if(init)
	{
		float shadow_edge[10] = {0.0, 0.0, -0.005, 0.005, -0.005, 0.02, 0.005, 0.02, 0.005, 0.005};
		sui_create_shadow_edge(0.02, 5, shadow, color, shadow_edge);
		init = TRUE;
	}
	if(select)
		f = 1.0;
	else
		f = 0.7;
	glPushMatrix();
	glTranslated(pos_x, pos_y, 0);
	sui_draw_gl(GL_TRIANGLES, polys, 9, 2, f, f, f, 0.8);
	sui_set_color_array_gl(color, 5 * 4, 4);
	sui_draw_gl(GL_QUADS, shadow, 5 * 4, 2, 1.0, 1.0, 1.0, 0.8);
	glPopMatrix();
}

void ca_draw_key_end(float pos_x, float pos_y, float side, boolean select)
{
	static float shadow[2 * 4 * 5], color[4 * 4 * 5], polys[8] = {0.0, 0.0, -0.01, 0.01, -0.01, 0.04, 0.0, 0.04}, f;
	static boolean init = TRUE;
	if(init)
	{
		float shadow_edge[8] = {0.0, 0.0, -0.005, 0.005, -0.005, 0.02, 0.0, 0.02};
		sui_create_shadow_edge(0.02, 4, shadow, color, polys);
		init = TRUE;
	}
	if(select)
		f = 1.0;
	else
		f = 0.7;
	glPushMatrix();
	glTranslated(pos_x, pos_y, 0);
	glScalef(side, 1, 1);
	sui_draw_gl(GL_QUADS, polys, 4, 2, f, f, f, 0.8);
	sui_set_color_array_gl(color, 4 * 4, 4);
	sui_draw_gl(GL_QUADS, shadow, 4 * 4, 2, 1.0, 1.0, 1.0, 0.8);
	glPopMatrix();
}

void ca_draw_timeline(BInputState *input, CCharDesc *desc, CCharacter *c, float delta)
{
	static uint grab = -1, trasition_grab = -1, edge_grab = -1;
	float aspect, f, f2, pos[2];
	uint i, j;
	if(ca_animator_selected_clip == -1)
		return;
	aspect = betray_get_screen_mode(NULL, NULL, NULL);


	if(input->mode == BAM_DRAW)
	{
		float array[4 * 2] = {1, 0, -1, 0, 
							-1, -10, 1, -10};
		array[1] = array[3] = -aspect + 0.16;
		sui_draw_gl(GL_QUADS, array, 4, 2, 0.17, 0.17, 0.17, 0.8);

		sui_draw_3d_line_gl(0.65, -aspect + 0.09, 0, 
							-0.65, -aspect + 0.09, 0, 
							1.0, 1.0, 1.0, 0.2);
		sui_draw_3d_line_gl(0.65, -aspect + 0.1, 0, 
							-0.65, -aspect + 0.1, 0, 
							1.0, 1.0, 1.0, 0.2);

		sui_draw_3d_line_gl(-0.65, -aspect + 0.09, 0, 
							-0.65 + desc->events[ca_animator_selected_clip].start * 0.65 * 2.0, -aspect + 0.1, 0, 
							0.3, 0.8, 0.5, 1.0);

		sui_draw_3d_line_gl(-0.65 + desc->events[ca_animator_selected_clip].start * 0.65 * 2.0, -aspect + 0.10, 0, 
							-0.65 + desc->events[ca_animator_selected_clip].start * 0.65 * 2.0, -aspect + 0.09, 0, 
							1.0, 1.0, 1.0, 0.2);


		sui_draw_3d_line_gl(-0.65 + desc->events[ca_animator_selected_clip].start * 0.65 * 2.0, -aspect + 0.1, 0,
							-0.65 + desc->events[ca_animator_selected_clip].end * 0.65 * 2.0, -aspect + 0.1, 0, 
							0.3, 0.8, 0.5, 1.0);
	

		sui_draw_3d_line_gl(-0.65 + desc->events[ca_animator_selected_clip].end * 0.65 * 2.0, -aspect + 0.10, 0, 
							-0.65 + desc->events[ca_animator_selected_clip].end * 0.65 * 2.0, -aspect + 0.09, 0, 
							1.0, 1.0, 1.0, 0.2);

		sui_draw_3d_line_gl(0.65, -aspect + 0.09, 0, 
							-0.65 + desc->events[ca_animator_selected_clip].end * 0.65 * 2.0, -aspect + 0.1, 0, 
							0.3, 0.8, 0.5, 1.0);
	
		sui_draw_3d_line_gl(-0.65 + ca_animator_selected_time * 0.65 * 2.0, -aspect + 0.09, 0, 
							-0.65 + ca_animator_selected_time * 0.65 * 2.0, -aspect + 0.1, 0, 
							0.3, 0.8, 0.5, 1.0);

		sui_draw_3d_line_gl(-0.65 + ca_animator_selected_time * 0.65 * 2.0 + 0.02, -aspect + 0.13, 0, 
							-0.65 + ca_animator_selected_time * 0.65 * 2.0, -aspect + 0.11, 0, 
							1.0, 1.0, 1.0, 0.2);

		sui_draw_3d_line_gl(-0.65 + ca_animator_selected_time * 0.65 * 2.0 - 0.02, -aspect + 0.13, 0, 
							-0.65 + ca_animator_selected_time * 0.65 * 2.0, -aspect + 0.11, 0, 
							1.0, 1.0, 1.0, 0.2);


	}


//	void ca_draw_key_frame(float pos_x, float pos_y, boolean select)

	if(!input->mouse_button[0])
		grab = -1;
	for(i = 0; i < C_APC_COUNT; i++)
	{
		if(ca_animator_selected_point == i)
		{
			if(desc->events[ca_animator_selected_clip].anim[i].point_count > 1)
			{
				if(input->mode == BAM_DRAW)
				{
					ca_draw_key_end(-0.65 + desc->events[ca_animator_selected_clip].anim[i].points[0].time * 0.65 * 2.0, -aspect, -1, TRUE);
					ca_draw_key_end(-0.65 + desc->events[ca_animator_selected_clip].anim[i].points[desc->events[ca_animator_selected_clip].anim[i].point_count - 1].time * 0.65 * 2.0, -aspect, 1, TRUE);
				}else
				{	
					if(!input->mouse_button[0])
						edge_grab = -1;
					if(edge_grab == -1 && input->mouse_button[0] && !input->last_mouse_button[0] &&
						sui_box_click_test(-0.66 + desc->events[ca_animator_selected_clip].anim[i].points[0].time * 0.65 * 2.0, -aspect, 0.02, 0.04))
						edge_grab = 0;
					if(edge_grab == -1 && input->mouse_button[0] && !input->last_mouse_button[0] &&
						sui_box_click_test(-0.66 + desc->events[ca_animator_selected_clip].anim[i].points[desc->events[ca_animator_selected_clip].anim[i].point_count - 1].time * 0.65 * 2.0, -aspect, 0.02, 0.04))
						edge_grab = 1;
					if(edge_grab == 0)
					{
						 f = (input->pointer_x + 0.65) / (0.65 * 2.0);
						 if(f < 0.0)
							 f = 0;
						 if(f > desc->events[ca_animator_selected_clip].anim[i].points[desc->events[ca_animator_selected_clip].anim[i].point_count - 1].time - 0.01)
							 f = desc->events[ca_animator_selected_clip].anim[i].points[desc->events[ca_animator_selected_clip].anim[i].point_count - 1].time - 0.01;

						 f2 = desc->events[ca_animator_selected_clip].anim[i].points[desc->events[ca_animator_selected_clip].anim[i].point_count - 1].time;
						 f = (f - f2) / (desc->events[ca_animator_selected_clip].anim[i].points[0].time - f2);
						 for(j = 0; j < desc->events[ca_animator_selected_clip].anim[i].point_count - 1; j++)
							 desc->events[ca_animator_selected_clip].anim[i].points[j].time = f2 + (desc->events[ca_animator_selected_clip].anim[i].points[j].time - f2) * f;
					}
					if(edge_grab == 1)
					{
						 f = (input->pointer_x + 0.65) / (0.65 * 2.0);

						 if(f < desc->events[ca_animator_selected_clip].anim[i].points[0].time + 0.01)
							 f = desc->events[ca_animator_selected_clip].anim[i].points[0].time + 0.01;
						 if(f > 1.0)
							 f = 1.0;
						 f2 = desc->events[ca_animator_selected_clip].anim[i].points[0].time;
						 f = (f - f2) / (desc->events[ca_animator_selected_clip].anim[i].points[desc->events[ca_animator_selected_clip].anim[i].point_count - 1].time - f2);
						 for(j = 1; j < desc->events[ca_animator_selected_clip].anim[i].point_count; j++)
							 desc->events[ca_animator_selected_clip].anim[i].points[j].time = f2 + (desc->events[ca_animator_selected_clip].anim[i].points[j].time - f2) * f;
					}
				}
			}
			for(j = 0; j < desc->events[ca_animator_selected_clip].anim[i].point_count; j++)
			{
				f = desc->events[ca_animator_selected_clip].anim[i].points[j].time;
				if(input->mode == BAM_DRAW)
				{
					sui_manipulator_cross_draw(-0.65 + f * 0.65 * 2.0, -aspect + 0.15, 0.8, 0.8, 0.8);
					ca_draw_key_frame(-0.65 + f * 0.65 * 2.0, -aspect + 0.11, ca_animator_selected_key == j || sui_box_click_test(-0.66 + f * 0.65 * 2.0, -aspect + 0.13, 0.02, 0.04));
				}else
				{	

					if(grab == j)
					{
						f = (input->pointer_x + 0.65) / (0.65 * 2.0);
						if(f > 1.0)
							f = 1.0;
						if(f < 0.0)
							f = 0.0;
						if(j != 0 && f < desc->events[ca_animator_selected_clip].anim[i].points[j - 1].time + 0.01)
							f = desc->events[ca_animator_selected_clip].anim[i].points[j - 1].time + 0.01;
						if(j != desc->events[ca_animator_selected_clip].anim[i].point_count - 1 &&
							f > desc->events[ca_animator_selected_clip].anim[i].points[j + 1].time - 0.01)
							f = desc->events[ca_animator_selected_clip].anim[i].points[j + 1].time - 0.01;
						desc->events[ca_animator_selected_clip].anim[i].points[j].time = f;
					}
					if(grab == -1 && input->mouse_button[0] && !input->last_mouse_button[0] &&
						sui_box_click_test(-0.66 + f * 0.65 * 2.0, -aspect + 0.11, 0.02, 0.02))
						grab = j;
					if(grab == -1 && input->mouse_button[0] && !input->last_mouse_button[0] &&
						sui_box_click_test(-0.66 + f * 0.65 * 2.0, -aspect + 0.14, 0.02, 0.02))
						c_character_desc_key_destroy(desc, ca_animator_selected_clip, i, j);
				}
			}
		}
	}
	if(input->mode == BAM_EVENT)
	{
		if(/*grab == -1 && */input->mouse_button[0] && !input->last_mouse_button[0])
		{
			trasition_grab = -1;
			if(sui_box_click_test(-0.67 + desc->events[ca_animator_selected_clip].start * 0.65 * 2.0, -aspect + 0.09, 0.04, 0.01))
				trasition_grab = 0;
			if(sui_box_click_test(-0.67 + desc->events[ca_animator_selected_clip].end * 0.65 * 2.0, -aspect + 0.09, 0.04, 0.01))
				trasition_grab = 1;
			if(trasition_grab != -1)
				grab = -1;
		}
		if(!input->mouse_button[0])
			trasition_grab = -1;
		
		if(trasition_grab != -1)
		{
			f = (input->pointer_x + 0.65) / (0.65 * 2.0);
			if(f > 1.0)
				f = 1.0;
			if(f < 0.0)
				f = 0.0;
			if(trasition_grab == 0)
				desc->events[ca_animator_selected_clip].start = f;
			if(trasition_grab == 1)
				desc->events[ca_animator_selected_clip].end = f;

		}
	}


	if(input->mode == BAM_DRAW && ca_animator_play)
	{
		ca_animator_selected_time += delta / desc->events[ca_animator_selected_clip].speed;
		if(ca_animator_selected_time > 1.0)
			ca_animator_selected_time = 0;
	}

	if(input->mode == BAM_EVENT)
	{
		if(ca_animator_play && ca_animator_selected_clip != -1)
		{
			ca_animator_selected_time += delta / desc->events[ca_animator_selected_clip].speed;
			if(ca_animator_selected_time > 1.0)
				ca_animator_selected_time = 0;
		}
		if(trasition_grab == -1)
		{
			if(input->click_pointer_y < -aspect + 0.13 &&
				input->click_pointer_x < 0.65 &&
				input->click_pointer_x > -0.65 &&
				input->mouse_button[0])
			{
				ca_animator_selected_time = (input->pointer_x + 0.65) / (0.65 * 2.0);
				if(ca_animator_selected_time > 1.0)
					ca_animator_selected_time = 1.0;
				if(ca_animator_selected_time < 0.0)
					ca_animator_selected_time = 0.0;
				if(input->mouse_button[0])
					ca_animator_play = FALSE;
			}
		}
	}

	
	if(input->mode == BAM_DRAW)
	{
		for(i = 0; i < 129; i++)
		{
			f = 0.01;
			if(i % 2 == 0)
				f += 0.01;
			if(i % 8 == 0)
				f += 0.01;
			sui_draw_3d_line_gl(-0.65 + ((float)i / 64.0) * 0.65, -aspect + 0.08, 0, 
								-0.65 + ((float)i / 64.0) * 0.65, -aspect + 0.08 - f, 0, 
								1.0, 1.0, 1.0, 0.2);
		}


		if(input->pointer_x > -0.85 - 0.1375 && input->pointer_x < -0.85 + 0.1375 &&
			input->pointer_y > -aspect + 0.11 && input->pointer_y < -aspect + 0.15)
			sui_draw_window(-0.85, -aspect + 0.14, 0.25, 0.02, 0.01, 1.0, 0.2, 0.2, 1, NULL); 
		else
			sui_draw_window(-0.85, -aspect + 0.14, 0.25, 0.02, 0.01, 0.2, 0.2, 0.2, 1, NULL); 			
		sui_draw_text(-0.85 + sui_text_line_length(SUI_T_SIZE, SUI_T_SPACE, "DELETE", -1) * -0.5, -aspect + 0.12, SUI_T_SIZE, SUI_T_SPACE, "DELETE", 0, 0, 0, 0.5);

		sui_draw_window(-0.85, -aspect + 0.09, 0.25, 0.02, 0.01, 0.2, 0.2, 0.2, 1, NULL); 
		sui_draw_text(-0.85 + sui_text_line_length(SUI_T_SIZE, SUI_T_SPACE, "CLEAR", -1) * -0.5, -aspect + 0.07, SUI_T_SIZE, SUI_T_SPACE, "CLEAR", 0, 0, 0, 0.5);		


		if(ca_character_stroke_rec)
			sui_draw_window(0.85, -aspect + 0.14, 0.25, 0.02, 0.01, 0.3, 0.8, 0.5, 1, NULL);
		else
			sui_draw_window(0.85, -aspect + 0.14, 0.25, 0.02, 0.01, 0.2, 0.2, 0.2, 1, NULL); 
		sui_draw_text(0.85 + sui_text_line_length(SUI_T_SIZE, SUI_T_SPACE, "REC", -1) * -0.5, -aspect + 0.12, SUI_T_SIZE, SUI_T_SPACE, "REC", 0, 0, 0, 0.5);

		if(desc->events[ca_animator_selected_clip].loop)
			sui_draw_window(0.85, -aspect + 0.09, 0.25, 0.02, 0.01, 0.4, 0.4, 0.4, 1, NULL); 
		else
			sui_draw_window(0.85, -aspect + 0.09, 0.25, 0.02, 0.01, 0.2, 0.2, 0.2, 1, NULL); 
		sui_draw_text(0.85 + sui_text_line_length(SUI_T_SIZE, SUI_T_SPACE, "LOOP", -1) * -0.5, -aspect + 0.07, SUI_T_SIZE, SUI_T_SPACE, "LOOP", 0, 0, 0, 0.5);
				
		pos[0] = 0.85 - 0.125 + 0.25 * desc->events[ca_animator_selected_clip].speed / 10.0;
		pos[1] = -aspect + 0.03;
		sui_manipulator_point_draw(pos[0], pos[1], 1.0, 0.3, 0.3, 0.3);
		
		if(pos[0] + 0.025 < 0.85 + 0.125)
		{
			sui_draw_3d_line_gl(pos[0] + 0.025, -aspect + 0.035, 0, 
							0.85 + 0.125, -aspect + 0.035, 0, 
								1.0, 1.0, 1.0, 0.2);
			sui_draw_3d_line_gl(pos[0] + 0.025, -aspect + 0.025, 0, 
							0.85 + 0.125, -aspect + 0.025, 0, 
								1.0, 1.0, 1.0, 0.2);
		}
		if(pos[0] - 0.025 > 0.85 - 0.125)
		{
			sui_draw_3d_line_gl(pos[0] - 0.025, -aspect + 0.035, 0, 
							0.85 - 0.125, -aspect + 0.035, 0, 
								1.0, 1.0, 1.0, 0.2);
			sui_draw_3d_line_gl(pos[0] - 0.025, -aspect + 0.025, 0, 
							0.85 - 0.125, -aspect + 0.025, 0, 
								1.0, 1.0, 1.0, 0.2);
		}
		
		sui_draw_3d_line_gl(-0.985, -aspect + 0.025, 0, 
							-0.985 + 0.27, -aspect + 0.025, 0, 
								1.0, 1.0, 1.0, 0.2);
	//	sui_draw_window(0.85, -aspect + 0.04, 0.2, 0.02, 0.01, 0.2, 0.2, 0.2, 1, NULL);
	}

	sui_type_in(input, -0.98, -aspect + 0.03, 0.275, SUI_T_SIZE, desc->events[ca_animator_selected_clip].name, 32, NULL, NULL, 0.3, 0.8, 0.5, 1);

	if(input->mode == BAM_EVENT)
	{
		if(input->mouse_button[0] && !input->last_mouse_button[0])
			if(input->pointer_x > 0.85 - 0.1375 && input->pointer_x < 0.85 + 0.1375)
				if(input->pointer_y > -aspect + 0.06 && input->pointer_y < -aspect + 0.1)
					desc->events[ca_animator_selected_clip].loop = !desc->events[ca_animator_selected_clip].loop;

		if(input->mouse_button[0] && !input->last_mouse_button[0])
			if(input->pointer_x > 0.85 - 0.1375 && input->pointer_x < 0.85 + 0.1375)
				if(input->pointer_y > -aspect + 0.11 && input->pointer_y < -aspect + 0.15)
					ca_character_stroke_rec = !ca_character_stroke_rec;

		if(input->mouse_button[0] && !input->last_mouse_button[0])
			if(input->pointer_x > -0.85 - 0.1375 && input->pointer_x < -0.85 + 0.1375)
				if(input->pointer_y > -aspect + 0.06 && input->pointer_y < -aspect + 0.1)
					desc->events[ca_animator_selected_clip].anim[ca_animator_selected_point].point_count = 0;



		if(input->mouse_button[0])
			if(input->click_pointer_x > 0.85 - 0.1375 && input->click_pointer_x < 0.85 + 0.1375)
				if(input->click_pointer_y > -aspect + 0.01 && input->click_pointer_y < -aspect + 0.05)
					desc->events[ca_animator_selected_clip].speed = (input->pointer_x - (0.85 - 0.125)) / 0.025;

		if(desc->events[ca_animator_selected_clip].speed > 10.0)
			desc->events[ca_animator_selected_clip].speed = 10.0;
		if(desc->events[ca_animator_selected_clip].speed < 0.0)
			desc->events[ca_animator_selected_clip].speed = 0.0;

		if(input->mouse_button[0] && !input->last_mouse_button[0])
		{
			if(input->pointer_x > -0.85 - 0.1375 && input->pointer_x < -0.85 + 0.1375)
			{
				if(input->pointer_y > -aspect + 0.11 && input->pointer_y < -aspect + 0.15)
				{
					c_character_desc_event_delete(desc, ca_animator_selected_clip);
					ca_animator_selected_key = -1;
					ca_animator_selected_clip = -1;
				}
			}
		}

	}
	c_character_event_over_ride_time(ca_animator_selected_time);
}


