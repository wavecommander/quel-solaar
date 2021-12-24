#include <math.h>

#include "st_matrix_operations.h"
#include "seduce.h"
#include "confuse.h"
#include "cs.h"

static boolean ca_show_setup_panel = FALSE;
float ca_animator_walk_speed = 0;

void ca_draw_popup(CCharDesc *desc,  CCharacter *c, BInputState *input)
{
	static float pos_x, pos_y;
	static double slider = 0.75;
	SUIPUElement element[60];
	uint out, i, j, wide, height, count;
	if(input->mouse_button[1])
		return;
	if(ca_animator_selected_clip != -1 && ca_animator_selected_point != -1)
		element[0].text = "NEXT KEY";
	else
		element[0].text = "";
	element[0].type = PU_T_ANGLE;
	element[0].data.angle[0] = 45.0;
	element[0].data.angle[1] = 45.0 + 90.0;
	if(ca_animator_selected_clip != -1 && ca_animator_selected_point != -1)
		element[1].text = "PREVIOUS KEY";
	else
		element[1].text = "";
	element[1].type = PU_T_ANGLE;
	element[1].data.angle[0] = 180.0 + 45.0;
	element[1].data.angle[1] = 180.0 + 45.0 + 90.0;

	element[2].text = "ADD CLIP";
	element[2].type = PU_T_BOTTOM;

	if(ca_show_setup_panel)
		element[3].text = "HIDE SETUP";
	else
		element[3].text = "SHOW SETUP";
	element[3].type = PU_T_BOTTOM;

	element[4].text = "EXIT";
	element[4].type = PU_T_BOTTOM;

	element[5].text = "SPEED";
	element[5].type = PU_T_HSLIDER,
	element[5].data.slider.pos[0] = -0.1;
	element[5].data.slider.pos[1] = -0.325;
	element[5].data.slider.length = 0.2;
	slider = ca_animator_walk_speed;
	element[5].data.slider.value = &slider;

	element[6].text = "POSE";
	element[6].type = PU_T_SQUARE;
	element[6].data.square.square[0] = -0.1;
	element[6].data.square.square[1] = 0.20;
	element[6].data.square.square[2] = 0.20;
	element[6].data.square.square[3] = -0.06;
	element[6].data.square.draw_func = NULL;

	count = 7;
	wide = height = 1;
	for(i = 0; i < desc->event_count; )
	{
		wide++;
		height++;
		if(i + wide > desc->event_count)
			wide = desc->event_count - i;
		for(j = 0; j < wide && i < desc->event_count; j++)
		{
			element[count].text = desc->events[i].name;
			element[count].type = PU_T_SQUARE;
			element[count].data.square.square[0] = 0.16 * (float)j - 0.08 * (float)(wide);
			element[count].data.square.square[1] = 0.05 * (float)height + 0.15;
			element[count].data.square.square[2] = 0.15;
			element[count].data.square.square[3] = -0.04;
			element[count].data.square.draw_func = NULL;
			count++;
			i++;
		}
	}

	if(input->mouse_button[2] && !input->last_mouse_button[2])
	{
		pos_x = input->pointer_x;
		pos_y = input->pointer_y;
	}
	if(input->mouse_button[2] || input->last_mouse_button[2])
	{
		if(input->mode == BAM_EVENT && !input->mouse_button[2])
			if((pos_x - input->pointer_x) * (pos_x - input->pointer_x) + (pos_y - input->pointer_y) * (pos_y - input->pointer_y) < 0.004 * 0.004)
				ca_animator_play = !ca_animator_play;
		out = sui_draw_popup(input, pos_x, pos_y, element, count, 2, 0.0);
		if(out != -1)
		{
			switch(out)
			{
				case 0 :
					if(ca_animator_selected_clip != -1 && ca_animator_selected_point != -1)
					{
						for(i = 0; i < desc->events[ca_animator_selected_clip].anim[ca_animator_selected_point].point_count - 1; i++)
							if(ca_animator_selected_time < -0.001 + desc->events[ca_animator_selected_clip].anim[ca_animator_selected_point].points[i].time)
								break;
						ca_animator_selected_time = desc->events[ca_animator_selected_clip].anim[ca_animator_selected_point].points[i].time;
					}
				break;
				case 1 :
					if(ca_animator_selected_clip != -1 && ca_animator_selected_point != -1)
					{
						for(i = desc->events[ca_animator_selected_clip].anim[ca_animator_selected_point].point_count - 1; i != 0; i--)
							if(ca_animator_selected_time > 0.001 + desc->events[ca_animator_selected_clip].anim[ca_animator_selected_point].points[i].time)
								break;
						ca_animator_selected_time = desc->events[ca_animator_selected_clip].anim[ca_animator_selected_point].points[i].time;
					}
				break;
				case 2 :
					ca_animator_selected_clip = c_character_desc_event_create(desc);
				break;
				case 3 :
					ca_show_setup_panel = !ca_show_setup_panel;
				break;
				case 4 :
					cs_save(&ca_description);
					exit(0);
				break;
				case 6:
					ca_animator_selected_key = -1;
					ca_animator_selected_clip = -1;
				break;
			}
			if(out >= 7)
			{
				ca_animator_selected_clip = out - 7;
			}
		}
		ca_animator_walk_speed = slider;
	}
}

//	c_character_desc_key_destroy(desc, ca_animator_selected_clip, ca_animator_selected_point, ca_animator_selected_key);


boolean ca_draw_settings_menu(BInputState *input, CCharDesc *c)
{
	static float size = 2, pos_x = 0, pos_y = 0;
	static boolean grab = FALSE;
	SUIViewElement element[10];

	if(!ca_show_setup_panel)
		return FALSE;

	element[0].type = S_VET_SLIDER;
	element[0].text = "Size";
	element[0].param.slider = size * 0.1;

	element[1].type = S_VET_SLIDER;
	element[1].text = "Head";
	element[1].param.slider = c->head_size / size;

	element[2].type = S_VET_SLIDER;
	element[2].text = "Neck";
	element[2].param.slider = c->neck_length / size;

	element[3].type = S_VET_SLIDER;
	element[3].text = "Torso";
	element[3].param.slider = c->torso_size / size;

	element[4].type = S_VET_SLIDER;
	element[4].text = "Torso";
	element[4].param.slider = c->torso_width / size;

	element[5].type = S_VET_SLIDER;
	element[5].text = "Stomach";
	element[5].param.slider = c->stomach_size / size;

	element[6].type = S_VET_SLIDER;
	element[6].text = "Butt";
	element[6].param.slider = c->butt_size / size;

	element[7].type = S_VET_SLIDER;
	element[7].text = "Arms";
	element[7].param.slider = c->arms_size / size;

	element[8].type = S_VET_SLIDER;
	element[8].text = "Legs";
	element[8].param.slider = c->legs_size / size;

	element[9].type = S_VET_SLIDER;
	element[9].text = "Timer";
	element[9].param.slider = c->legs_size / size;

	if(input->mode == BAM_EVENT)
	{

		if(input->mouse_button[0] && !input->last_mouse_button[0] && 
			input->pointer_x > pos_x - 0.12 && input->pointer_x < pos_x + 0.12 &&
			input->pointer_y > pos_y - 0.01 && input->pointer_y < pos_y + 0.03)
			grab = TRUE;
		if(!input->mouse_button[0])
			grab = FALSE;
		if(grab)
		{
			pos_x += input->delta_pointer_x;
			pos_y += input->delta_pointer_y;
		}
	}

	if(sui_draw_setting_view(input, pos_x, pos_y, 0.2, 0.7, element, 10, "Setup", 0.017))
	{
		return input->pointer_x > pos_x - 0.12 && input->pointer_x < pos_x + 0.12 &&
			input->pointer_y > pos_y - 0.38 && input->pointer_y < pos_y + 0.03;
	}
	if(!grab)
	{
		size = element[0].param.slider / 0.1;
		if(size < 0.01)
			size = 0.01;
		c->head_size = element[1].param.slider * size;
		c->neck_length = element[2].param.slider * size;
		c->torso_size = element[3].param.slider * size;
		c->torso_width = element[4].param.slider * size;
		c->stomach_size = element[5].param.slider * size;
		c->butt_size = element[6].param.slider * size;
		c->arms_size = element[7].param.slider * size;
		c->legs_size = element[8].param.slider * size;
	}
	return input->pointer_x > pos_x - 0.12 && input->pointer_x < pos_x + 0.12 &&
		input->pointer_y > pos_y - 0.38 && input->pointer_y < pos_y + 0.03;
}

boolean ca_draw_menu_param(BInputState *input, CCharDesc *desc, CCharacter *c, uint selected)
{
	SUIViewElement element[14];
	CAnimPoint point;
	uint space;

	ca_input_animator_edit_point_get(desc, c, &point, selected);
	
	element[0].type = S_VET_REAL;
	element[0].text = "X";
	element[0].param.real = point.pos[0];

	element[1].type = S_VET_REAL;
	element[1].text = "Y";
	element[1].param.real = point.pos[1];

	element[2].type = S_VET_REAL;
	element[2].text = "Z";
	element[2].param.real = point.pos[2];

	element[3].type = S_VET_BOOLEAN;
	element[3].text = "reset";
	element[3].param.checkbox = FALSE;

	element[4].type = S_VET_SPLIT;
	element[4].text = "space";

	element[5].type = S_VET_BOOLEAN;
	element[5].text = "Base";
	element[5].param.checkbox = point.space == C_CM_BASE;

	element[6].type = S_VET_BOOLEAN;
	element[6].text = "Look";
	element[6].param.checkbox = point.space == C_CM_LOOK;

	element[7].type = S_VET_BOOLEAN;
	element[7].text = "Butt";
	element[7].param.checkbox = point.space == C_CM_BUTT;

	element[8].type = S_VET_BOOLEAN;
	element[8].text = "Belly";
	element[8].param.checkbox = point.space == C_CM_BELLY;

	element[9].type = S_VET_BOOLEAN;
	element[9].text = "Torso";
	element[9].param.checkbox = point.space == C_CM_TORSO;

	element[10].type = S_VET_BOOLEAN;
	element[10].text = "Head";
	element[10].param.checkbox = point.space == C_CM_HEAD;

	element[11].type = S_VET_BOOLEAN;
	element[11].text = "R Leg";
	element[11].param.checkbox = point.space == C_CM_LEG_RA_UP;

	element[12].type = S_VET_BOOLEAN;
	element[12].text = "L Leg";
	element[12].param.checkbox = point.space == C_CM_LEG_LA_UP;

	element[13].type = S_VET_BOOLEAN;
	element[13].text = "Tool";
	element[13].param.checkbox = point.space == C_CM_TOOL;

	if(sui_draw_setting_view(input, 0.85, betray_get_screen_mode(NULL, NULL, NULL) - 0.05, 0.2, 0.5, element, 14, "Param", 0.0))
	{
		return TRUE;
	}
	point.pos[0] = element[0].param.real;
	point.pos[1] = element[1].param.real;
	point.pos[2] = element[2].param.real;

	if(element[3].param.checkbox)
	{
		point.pos[0] = 0;
		point.pos[1] = 0;
		point.pos[2] = 0;
	}
	space = point.space;
	if(element[5].param.checkbox && point.space != C_CM_BASE)
		space = C_CM_BASE;
	if(element[6].param.checkbox && point.space != C_CM_LOOK)
		space = C_CM_LOOK;
	if(element[7].param.checkbox && point.space != C_CM_BUTT)
		space = C_CM_BUTT;
	if(element[8].param.checkbox && point.space != C_CM_BELLY)
		space = C_CM_BELLY;
	if(element[9].param.checkbox && point.space != C_CM_TORSO)
		space = C_CM_TORSO;
	if(element[10].param.checkbox && point.space != C_CM_HEAD)
		space = C_CM_HEAD;
	if(element[11].param.checkbox && point.space != C_CM_LEG_RA_UP)
		space = C_CM_LEG_RA_UP;
	if(element[12].param.checkbox && point.space != C_CM_LEG_LA_UP)
		space = C_CM_LEG_LA_UP;
	if(element[13].param.checkbox && point.space != C_CM_TOOL)
		space = C_CM_TOOL;

	if(point.space != space)
	{
		float tmp[3];
		c_character_from_matrix(tmp, c, &point, selected);
		point.pos[0] = tmp[0];
		point.pos[1] = tmp[1];
		point.pos[2] = tmp[2];
		point.space = space;
		c_character_to_matrix(tmp, c, &point, selected);
		point.pos[0] = tmp[0];
		point.pos[1] = tmp[1];
		point.pos[2] = tmp[2];
	}
//	ca_input_animator_edit_point_set(desc, c, &point, selected);
	return TRUE;
}


boolean ca_draw_menu_movement(BInputState *input, float *speed)
{
	SUIViewElement element[15];
	
	element[0].type = S_VET_SLIDER;
	element[0].text = "";
	element[0].param.slider = *speed;
	if(sui_draw_setting_view(input, 0.85, betray_get_screen_mode(NULL, NULL, NULL) - 0.8, 0.2, 0.5, element, 1, "Speed", 0.0))
	{
		return TRUE;
	}
	*speed = element[0].param.slider;
	return TRUE;
}

extern uint ca_animator_selected_clip;

boolean ca_draw_anim_menu(BInputState *input, CCharDesc *c)
{
	static uint last_select = -1;
	SUIViewElement element[10];
	if(ca_animator_selected_clip == -1)
	{
		if(last_select == -1)
			return FALSE;
	}else
		last_select = ca_animator_selected_clip;

	element[0].type = S_VET_SLIDER;
	element[0].text = "Time";
	element[0].param.slider = c->events[last_select].speed / 10.0;

	element[1].type = S_VET_BOOLEAN;
	element[1].text = "loop";
	element[1].param.checkbox = c->events[last_select].loop;

	element[2].type = S_VET_TEXT;
	element[2].text = "Type";
	element[2].param.text.text = c->events[last_select].name;
	element[2].param.text.length = 32;

	if(sui_draw_setting_view(input, 0.85, betray_get_screen_mode(NULL, NULL, NULL) - 0.9, 0.2, 0.5, element, 3, "Setup", 0.0))
	{
		return TRUE;
	}
	c->events[last_select].speed = element[0].param.slider * 10.0;
	c->events[last_select].loop = element[1].param.checkbox;
	return TRUE;
}

