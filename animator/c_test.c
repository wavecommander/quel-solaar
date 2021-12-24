#include <math.h>
#include "betray.h"
#include "confuse.h"
#include "me_shared_system_files.h"
#include "me_client_character_render.h"


extern float c_test_path[];


void c_character_type_create(CCharDesc *c, float size)
{
//	MESSCharacter *c;
//	char name[16];
//	uint token[8];
//	uint8 head;
	c->head_size = size * 0.035156;
//	uint8 torso;
	c->torso_size = size * 0.044922;
	c->torso_width = size * 0.064453;
//	uint8 stomach;
	c->stomach_size = size * 0.074219;
//	uint8 butt;
	c->butt_size = size * 0.0350453;
	c->butt_width = size * 0.0350453;
//	uint8 arms;
	c->arms_size = size * 0.152344;
//	uint8 legs;
	c->legs_size = size * 0.182344;
//	uint8 back_legs;
//	c->back_legs_size = size * 0.182344;
//	uint8 extra;
//	c->extra_size = size * 0.182344;
//	boolean active;
}

void c_draw_matrix(float *matrix);
void c_draw_line(float x0, float y0, float z0, float x1, float y1, float z1, float red, float green, float blue);

void c_draw_bone_line(CCharacter *c, uint m1, uint m2)
{
	c_draw_line(c->matrix[m1][12], c->matrix[m1][13], c->matrix[m1][14], c->matrix[m2][12], c->matrix[m2][13], c->matrix[m2][14], 0, 1, 1);
}

void c_draw_bone_end(CCharacter *c, uint m, float color)
{
	c_draw_line(c->matrix[m][12], c->matrix[m][13], c->matrix[m][14], c->matrix[m][12] - c->matrix[m][4] * 0.5, c->matrix[m][13] - c->matrix[m][5] * 0.5, c->matrix[m][14] - c->matrix[m][6] * 0.5, 0, 1.0, color);
}

extern void c_draw_pos(float *pos);


void c_draw_character_bones(CCharDesc *d, CCharacter *c)
{
	c_draw_bone_line(c, C_CM_BUTT, C_CM_BELLY);
//	c_draw_bone_line(c, C_CM_TORSO, C_CM_BELLY);
	c_draw_bone_line(c, C_CM_TORSO, C_CM_HEAD);

	if(d->torso_config == C_TC_NO_TORSO || d->torso_config == C_TC_NO_TORSO_OR_ARMS)
	{
		c_draw_line(c->matrix[C_CM_BUTT][12], c->matrix[C_CM_BUTT][13], c->matrix[C_CM_BUTT][14], c->matrix[C_CM_BELLY][12] + c->matrix[C_CM_BELLY][4], c->matrix[C_CM_BELLY][13] + c->matrix[C_CM_BELLY][5], c->matrix[C_CM_BELLY][14] + c->matrix[C_CM_BELLY][6], 0, 1, 1);
		c_draw_line(c->matrix[C_CM_TORSO][12], c->matrix[C_CM_TORSO][13], c->matrix[C_CM_TORSO][14], c->matrix[C_CM_BELLY][12] + c->matrix[C_CM_BELLY][4], c->matrix[C_CM_BELLY][13] + c->matrix[C_CM_BELLY][5], c->matrix[C_CM_BELLY][14] + c->matrix[C_CM_BELLY][6], 0, 1, 1);
	}

	/* works */
/*	c_draw_line(c->matrix[C_CM_HEAD][12], c->matrix[C_CM_HEAD][13], c->matrix[C_CM_HEAD][14], c->matrix[C_CM_HEAD][12] - c->matrix[C_CM_HEAD][4] * 4, c->matrix[C_CM_HEAD][13] - c->matrix[C_CM_HEAD][5] * 4, c->matrix[C_CM_HEAD][14] - c->matrix[C_CM_HEAD][6] * 4, 0, 0.5, 1);
	c_draw_line(c->matrix[C_CM_HEAD][12], c->matrix[C_CM_HEAD][13], c->matrix[C_CM_HEAD][14], c->matrix[C_CM_HEAD][12] + c->matrix[C_CM_HEAD][0], c->matrix[C_CM_HEAD][13] + c->matrix[C_CM_HEAD][1], c->matrix[C_CM_HEAD][14] + c->matrix[C_CM_HEAD][2], 0, 0.5, 1);

	c_draw_line(c->matrix[C_CM_HEAD][12] - c->matrix[C_CM_HEAD][8], c->matrix[C_CM_HEAD][13] - c->matrix[C_CM_HEAD][9], c->matrix[C_CM_HEAD][14] - c->matrix[C_CM_HEAD][10],
				c->matrix[C_CM_HEAD][12] + c->matrix[C_CM_HEAD][8], c->matrix[C_CM_HEAD][13] + c->matrix[C_CM_HEAD][9], c->matrix[C_CM_HEAD][14] + c->matrix[C_CM_HEAD][10], 0, 1, 1);
*/

	/* should work 
	456 = 8910
	012 = 456
	8910 = 012 */
	c_draw_line(c->matrix[C_CM_HEAD][12], c->matrix[C_CM_HEAD][13], c->matrix[C_CM_HEAD][14], c->matrix[C_CM_HEAD][12] + c->matrix[C_CM_HEAD][8] * 4, c->matrix[C_CM_HEAD][13] + c->matrix[C_CM_HEAD][9] * 4, c->matrix[C_CM_HEAD][14] + c->matrix[C_CM_HEAD][10] * 4, 0, 0.5, 1);
	c_draw_line(c->matrix[C_CM_HEAD][12], c->matrix[C_CM_HEAD][13], c->matrix[C_CM_HEAD][14], c->matrix[C_CM_HEAD][12] + c->matrix[C_CM_HEAD][0], c->matrix[C_CM_HEAD][13] + c->matrix[C_CM_HEAD][1], c->matrix[C_CM_HEAD][14] + c->matrix[C_CM_HEAD][2], 0, 0.5, 1);

	c_draw_line(c->matrix[C_CM_HEAD][12] - c->matrix[C_CM_HEAD][0], c->matrix[C_CM_HEAD][13] - c->matrix[C_CM_HEAD][1], c->matrix[C_CM_HEAD][14] - c->matrix[C_CM_HEAD][2],
				c->matrix[C_CM_HEAD][12] + c->matrix[C_CM_HEAD][0], c->matrix[C_CM_HEAD][13] + c->matrix[C_CM_HEAD][1], c->matrix[C_CM_HEAD][14] + c->matrix[C_CM_HEAD][2], 0, 1, 1);

	c_draw_line(c->matrix[C_CM_HEAD][12], c->matrix[C_CM_HEAD][13], c->matrix[C_CM_HEAD][14], c->matrix[C_CM_HEAD][12] + c->look_dir[0], c->matrix[C_CM_HEAD][13] + c->look_dir[1], c->matrix[C_CM_HEAD][14] + c->look_dir[2], 0, 0.5, 1);



	if(d->torso_config < C_TC_NO_TORSO)
	{
		c_draw_bone_line(c, C_CM_TORSO, C_CM_ARM_LA_UP);
		c_draw_bone_line(c, C_CM_TORSO, C_CM_ARM_RA_UP);
		c_draw_bone_line(c, C_CM_ARM_LA_DOWN, C_CM_ARM_LA_UP);
		c_draw_bone_line(c, C_CM_ARM_RA_DOWN, C_CM_ARM_RA_UP);
		c_draw_bone_end(c, C_CM_ARM_LA_DOWN, 1.0);
		c_draw_bone_end(c, C_CM_ARM_RA_DOWN, 1.0);
	}
	if(d->torso_config == C_TC_NO_TORSO)
	{
		c_draw_bone_line(c, C_CM_BUTT, C_CM_ARM_LA_UP);
		c_draw_bone_line(c, C_CM_BUTT, C_CM_ARM_RA_UP);
		c_draw_bone_line(c, C_CM_ARM_LA_DOWN, C_CM_ARM_LA_UP);
		c_draw_bone_line(c, C_CM_ARM_RA_DOWN, C_CM_ARM_RA_UP);
		c_draw_bone_end(c, C_CM_ARM_LA_DOWN, 1.0);
		c_draw_bone_end(c, C_CM_ARM_RA_DOWN, 1.0);
	}

	if(d->torso_config == C_TC_DOUBLE_ARMS)
	{
		c_draw_bone_line(c, C_CM_ARM_LB_DOWN, C_CM_ARM_LB_UP);
		c_draw_bone_line(c, C_CM_ARM_RB_DOWN, C_CM_ARM_RB_UP);
		c_draw_bone_end(c, C_CM_ARM_LB_DOWN, 1.0);
		c_draw_bone_end(c, C_CM_ARM_RB_DOWN, 1.0);
	}
	if(d->leg_config >= C_LC_TWO)
	{
		c_draw_bone_line(c, C_CM_BUTT, C_CM_LEG_LA_UP);
		c_draw_bone_line(c, C_CM_BUTT, C_CM_LEG_RA_UP);
		c_draw_bone_line(c, C_CM_LEG_LA_DOWN, C_CM_LEG_LA_UP);
		c_draw_bone_line(c, C_CM_LEG_RA_DOWN, C_CM_LEG_RA_UP);
		c_draw_bone_end(c, C_CM_LEG_LA_DOWN, c->legs[0].timer);
		c_draw_bone_end(c, C_CM_LEG_RA_DOWN, c->legs[1].timer);
	}
	if(d->leg_config == C_LC_SIX)
	{
		c_draw_bone_line(c, C_CM_BUTT2, C_CM_LEG_LB_UP);
		c_draw_bone_line(c, C_CM_BUTT2, C_CM_LEG_RB_UP);
		c_draw_bone_line(c, C_CM_LEG_LB_DOWN, C_CM_LEG_LB_UP);
		c_draw_bone_line(c, C_CM_LEG_RB_DOWN, C_CM_LEG_RB_UP);
		c_draw_bone_end(c, C_CM_LEG_LB_DOWN, c->legs[2].timer);
		c_draw_bone_end(c, C_CM_LEG_RB_DOWN, c->legs[3].timer);
	}
	if(d->leg_config >= C_LC_FOUR)
	{
		c_draw_bone_line(c, C_CM_BUTT3, C_CM_LEG_RC_UP);
		c_draw_bone_line(c, C_CM_LEG_RC_DOWN, C_CM_LEG_RC_UP);
		c_draw_bone_end(c, C_CM_LEG_RC_DOWN, c->legs[5].timer);
		c_draw_bone_line(c, C_CM_BUTT3, C_CM_LEG_LC_UP);
		c_draw_bone_line(c, C_CM_LEG_LC_DOWN, C_CM_LEG_LC_UP);
		c_draw_bone_end(c, C_CM_LEG_LC_DOWN, c->legs[4].timer);
	}
	if(d->leg_config == C_LC_THREE)
	{
		c_draw_bone_line(c, C_CM_BUTT, C_CM_LEG_LC_UP);
		c_draw_bone_line(c, C_CM_LEG_LB_DOWN, C_CM_LEG_LB_UP);
		c_draw_bone_end(c, C_CM_LEG_LB_DOWN, c->legs[2].timer);
	}
	if(d->leg_config >= C_LC_FOUR)
	{	c_draw_bone_line(c, C_CM_BUTT, C_CM_BUTT2);
		c_draw_bone_line(c, C_CM_BUTT2, C_CM_BUTT3);
	}

	c_draw_pos(c->legs[0].foot_goal);
	c_draw_pos(c->legs[0].foot_pos);
	c_draw_pos(c->legs[1].foot_goal);
	c_draw_pos(c->legs[1].foot_pos);
	c_draw_pos(c->legs[2].foot_goal);
	c_draw_pos(c->legs[2].foot_pos);
	c_draw_pos(c->legs[3].foot_goal);
	c_draw_pos(c->legs[3].foot_pos);

}

extern float me_debug_sliders[100];

void c_character_create_humanoid(CCharDesc *c, float size, uint seed, float alien);
void c_character_create_creatiure(CCharDesc *c, float size, uint base_seed, uint id_seed);
MECharDraw *me_cr_dress(CCharDesc *desc, uint seed, uint id_seed);
void c_sim_run(CCharDesc *d, CCharacter *c, float delta, float *gravity);


void me_draw_character_test3(boolean sim)
{
	static CCharacter *c = NULL;
	static CCharDesc d;
	static MECharDraw *r;
	static float time = 1.1, rotate = 0;
	static uint change = 0;
	static uint path = 30;
	float look[3] = {-0.990074, -0.083869, -0.112784}, pos[3];
	uint i, inventory[2] = {C_IT_RIFLE, C_IT_IDLE};

	if(c == NULL)
	{
		c = malloc(sizeof(CCharacter)); 
		c_character_init(c, &c_test_path[path * 6 + 0]);
		c_character_type_create(&d, 0.06);
		me_debug_sliders[0] = 0.5;
		me_debug_sliders[1] = 0;
		me_debug_sliders[2] = 0.5;
	}


//	c_character_create_creatiure(&d, 0.05, change / 100, change / 100);
//	c_character_create_creatiure(&d, 0.015, 9, 9);
//	if(change == 0)
		
//		d.butt_width = 0.015 * me_debug_sliders[3];
//		d.fat = me_debug_sliders[0];
//		d.mussle = me_debug_sliders[2];
//	d.legs_lean[0] = me_debug_sliders[0] - 0.5;	
//	d.legs_lean[1] = me_debug_sliders[2] - 0.5;
	if(change % 30 == 0)
	{
		c_character_create_humanoid(&d, 0.015, change / 30, 0.01);
		r = me_cr_dress(&d, 0, change / 30, change / 30);
	}
	change++;


	time += me_debug_sliders[1];
	if(time > 1.0)
	{
		time -= 1.0;
		path = (path + 1) % 350;
	}

	look[0] = -(c_test_path[path * 6 + 3] * (1.0 - time) + c_test_path[((path + 1) % 350) * 6 + 3] * time);
	look[1] = -(c_test_path[path * 6 + 4] * (1.0 - time) + c_test_path[((path + 1) % 350) * 6 + 4] * time);
	look[2] = -(c_test_path[path * 6 + 5] * (1.0 - time) + c_test_path[((path + 1) % 350) * 6 + 5] * time);

	pos[0] = c_test_path[path * 6 + 0] * (1.0 - time) + c_test_path[((path + 1) % 350) * 6 + 0] * time;
	pos[1] = c_test_path[path * 6 + 1] * (1.0 - time) + c_test_path[((path + 1) % 350) * 6 + 1] * time;
	pos[2] = c_test_path[path * 6 + 2] * (1.0 - time) + c_test_path[((path + 1) % 350) * 6 + 2] * time;
/*
	pos[0] = c_test_path[30 * 6 + 0];
	pos[1] = c_test_path[30 * 6 + 1];
	pos[2] = c_test_path[30 * 6 + 2];
*/
	pos[0] *= 1.01;
	pos[1] *= 1.01;
	pos[2] *= 1.01;
	pos[0] = 0.618901;
	pos[1] = 0.622979;
	pos[2] = 0.485164;
	if(sim)
	{
		float gravity[3];
		gravity[0] = -pos[0] * 0.1;
		gravity[1] = -pos[1] * 0.1;
		gravity[2] = -pos[2] * 0.1;
		c_sim_run(&d, c, betray_get_delta_time() * 0.0, gravity);
	}//else
//		c_character_animate(c, &d, pos, pos, look, betray_get_delta_time());

//	me_cr_draw(r, c, TRUE);
//	c_draw_character_bones(&d, c);
}
