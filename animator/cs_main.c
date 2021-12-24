#include <math.h>

#include "st_matrix_operations.h"
#include "seduce.h"
#include "confuse.h"
#include "cs.h"

boolean ca_foot_plant_pos(float *goal, float *hip, float *vector, float *normal, float leg_length, void *user)
{
	float f;
	f = sqrt(vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2]);
	vector[0] /= f;
	vector[1] /= f;
	vector[2] /= f;
	goal[0] = hip[0] + vector[0] * hip[1] / -vector[1];
	goal[1] = 0;
	goal[2] = hip[2] + vector[2] * hip[1] / -vector[1];
	return TRUE;

}


void ca_context_update(void)
{
	glClearColor(0, 0, 0, 1);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	sui_set_blend_gl(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	sui_text_screen_mode_update();
}

void ca_input_handler(BInputState *input, void *user_pointer);

int main(int argc, char **argv)
{
	betray_init(argc, argv, 1700, 900, FALSE, "Animator");
	sui_init();
	glClearColor(0, 0, 0, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	c_character_plant_pos_func_set(ca_foot_plant_pos);
	betray_set_context_update_func(ca_context_update);
	betray_set_action_func(ca_input_handler, NULL);
	betray_launch_main_loop();
	cs_save(&ca_description);
	return 0;
}
