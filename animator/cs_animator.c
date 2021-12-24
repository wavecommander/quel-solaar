#include <math.h>

#include "st_matrix_operations.h"
#include "seduce.h"
#include "confuse.h"
#include "cs.h"




void ca_draw_events(BInputState *input, CCharacter *c, float x, float y, char *name, uint id)
{
	static uint last_select = -1;
	float f = 0;

	if(ca_animator_selected_clip != -1)
		last_select = ca_animator_selected_clip;
	if(input->mode == BAM_DRAW)
	{
		float vertex[] = {0.0, 0.0, 0.2, 0.0, 0.2, -0.03, 0.0, 0.0, 0.2, -0.03, 0.0, -0.03,
		0.205, 0.0, 0.22, -0.015,  0.205, -0.03,
		0.225, 0.0, 0.245, -0.0,  0.235, -0.03};
		glPushMatrix();
		glTranslatef(x, y, 0);
		if(id == ca_animator_selected_clip)
			f = 1.0;
		else if(last_select == id)
			f = 0.3;
		glColor4f(f, f, f, 0.5 + f * 0.5);
		glVertexPointer(2, GL_FLOAT, 0, vertex);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDrawArrays(GL_TRIANGLES, 6, 3);
		if(ca_animator_selected_tool == id)
			glColor4f(1, 1, 1, 1);
		else
			glColor4f(0, 0, 0, 0.3);
		if(-1 != id)
			glDrawArrays(GL_TRIANGLES, 9, 3);
		glPopMatrix();
		if(id != -1)
			sui_draw_text(x + 0.005, y - 0.021, SUI_T_SIZE, SUI_T_SPACE, "X", 1 - f, 1 - f, 1 - f, 1);
	}
	sui_draw_text(x + 0.035, y - 0.021, SUI_T_SIZE, SUI_T_SPACE, name, 1 - f, 1 - f, 1 - f, 1);
	if(input->mode == BAM_EVENT)
		if(input->mouse_button[0] && !input->last_mouse_button[0] &&
			sui_box_click_test(x, y - 0.03, 0.2, 0.03) &&
			sui_box_click_test(x, y - 0.03, 0.2, 0.03))
				ca_animator_selected_clip = id;

	if(input->mode == BAM_EVENT)
		if(input->mouse_button[0] && !input->last_mouse_button[0] &&
			sui_box_click_test(x + 0.205, y - 0.03, 0.02, 0.03) &&
			sui_box_click_test(x + 0.205, y - 0.03, 0.02, 0.03))
				c_character_event_play(c, id);

	if(input->mode == BAM_EVENT)
		if(input->mouse_button[0] && !input->last_mouse_button[0] &&
			sui_box_click_test(x + 0.225, y - 0.03, 0.03, 0.03) &&
			sui_box_click_test(x + 0.225, y - 0.03, 0.03, 0.03))
				ca_animator_selected_tool = id;
}

void ca_input_animator_edit_point_get(CCharDesc *desc, CCharacter *c, CAnimPoint *point, uint value_id)
{
	CAnimPoints *a;
	CAnimPoint p;
	if(ca_animator_selected_clip == -1)
		*point = desc->default_pose[value_id];
	else
	{
		a = &desc->events[ca_animator_selected_clip].anim[value_id];
		if(a->point_count != 0)
		{
			p = c->points[value_id];
			point->space = p.space = 0;
			c_character_to_matrix(point->pos, &ca_character, &p, ca_animator_selected_point);
			point->time = ca_animator_selected_time;
		}else
			*point = desc->default_pose[value_id];
	}

}

uint ca_input_animator_edit_point_set(CCharDesc *desc,  CCharacter *c, CAnimPoint *point, uint value_id)
{
	CAnimPoints *a;
	CAnimPoint *p;
	uint i;
	if(ca_animator_selected_clip == -1)
	{
		desc->default_pose[value_id] = *point;
		return -1;
	}
	else
	{
		a = &desc->events[ca_animator_selected_clip].anim[value_id];
		if(a->point_count != 0)
		{
			for(i = 0; i < desc->events[ca_animator_selected_clip].anim[value_id].point_count && (
				desc->events[ca_animator_selected_clip].anim[value_id].points[i].time > point->time + 0.01 ||
				desc->events[ca_animator_selected_clip].anim[value_id].points[i].time < point->time - 0.01); i++);
			if(i < desc->events[ca_animator_selected_clip].anim[value_id].point_count)
			{
				desc->events[ca_animator_selected_clip].anim[value_id].points[i] = *point;
				return i;
			}
			return c_character_desc_key_create(desc, ca_animator_selected_clip, value_id, point->time, point->space, point->pos);
		}
		c_character_desc_key_create(desc, ca_animator_selected_clip, value_id, point->time, point->space, point->pos);
		return 0;

	}
}

void ca_input_animator(BInputState *input, CCharDesc *desc, CCharacter *c)
{
	float aspect;
	uint i;
	aspect = betray_get_screen_mode(NULL, NULL, NULL);

	if(sw_text_button(input, -0.96, aspect - 0.04, 0, SUI_T_SIZE, SUI_T_SPACE, "ADD CLIP", 1.0, 1.0, 1.0, 1.0))
		c_character_desc_event_create(desc);

	ca_draw_events(input, c, -0.98, aspect - 0.06, "Default", -1);
	for(i = 0; i < desc->event_count; i++)
		ca_draw_events(input, c, -0.98, aspect - 0.1 + (float)i * -0.04, desc->events[i].name, i);
	c_character_event_over_ride(ca_animator_selected_clip);
}