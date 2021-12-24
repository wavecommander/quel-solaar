#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "st_matrix_operations.h"
#include "seduce.h"
#include "confuse.h"
#include "cs.h"


void cs_save(CCharDesc *desc)
{
	FILE *f;
	uint i, j, k;
	f = fopen("c_character_description.c", "w");
	printf("CS_SAVE!!!!!!!!!!!\n");

	fprintf(f, "#include <stdlib.h>\n");	
	fprintf(f, "#include \"st_types.h\"\n");
	fprintf(f, "#include \"confuse.h\"\n\n\n");

	fprintf(f, "void c_character_load(CCharDesc *desc, float scale)\n");
	fprintf(f, "{\n");
	fprintf(f, "\tuint i, j;\n");
	fprintf(f, "\tfloat pos[3];\n");
	fprintf(f, "\tdesc->head_size = %f * scale;\n", desc->head_size);
	fprintf(f, "\tdesc->neck_length = %f * scale;\n", desc->neck_length);
	fprintf(f, "\tdesc->neck_pos = %f * scale;\n", desc->neck_pos);
	fprintf(f, "\tdesc->torso_size = %f * scale;\n", desc->torso_size);
	fprintf(f, "\tdesc->torso_width = %f * scale;\n", desc->torso_width);
	fprintf(f, "\tdesc->torso_lean = %f * scale;\n", desc->torso_lean);
	fprintf(f, "\tdesc->stomach_size = %f * scale;\n", desc->stomach_size);
	fprintf(f, "\tdesc->butt_size = %f * scale;\n", desc->butt_size);
	fprintf(f, "\tdesc->butt_width = %f * scale;\n", desc->butt_width);
	fprintf(f, "\tdesc->body_length = %f * scale;\n", desc->body_length);
	fprintf(f, "\tdesc->arms_size = %f * scale;\n", desc->arms_size);
	fprintf(f, "\tdesc->legs_size = %f * scale;\n", desc->legs_size);
	fprintf(f, "\tdesc->legs_dir = %f * scale;\n", desc->legs_dir);
	fprintf(f, "\tdesc->scale = scale;\n");
	for(i = 0; i < C_APC_COUNT; i++)
	{
		if(desc->default_pose[i].space == C_CM_BASE)
		{
			fprintf(f, "\tdesc->default_pose[%u].pos[0] = %f * scale;\n", i, desc->default_pose[i].pos[0]);
			fprintf(f, "\tdesc->default_pose[%u].pos[1] = %f * scale;\n", i, desc->default_pose[i].pos[1]);
			fprintf(f, "\tdesc->default_pose[%u].pos[2] = %f * scale;\n", i, desc->default_pose[i].pos[2]);
		}else
		{
			fprintf(f, "\tdesc->default_pose[%u].pos[0] = %f;\n", i, desc->default_pose[i].pos[0]);
			fprintf(f, "\tdesc->default_pose[%u].pos[1] = %f;\n", i, desc->default_pose[i].pos[1]);
			fprintf(f, "\tdesc->default_pose[%u].pos[2] = %f;\n", i, desc->default_pose[i].pos[2]);
		}
		fprintf(f, "\tdesc->default_pose[%u].time = %f;\n", i, desc->default_pose[i].time);
		fprintf(f, "\tdesc->default_pose[%u].space = %u;\n", i, desc->default_pose[i].space);
	}
	fprintf(f, "\tdesc->event_count = %u;\n", desc->event_count);
	printf("CS_SAVE!!!!!!!!!!! %u \n", desc->event_count);
	if(desc->event_count != 0)
	{
		fprintf(f, "\tdesc->events = malloc((sizeof *desc->events) * desc->event_count);\n");
		fprintf(f, "\tfor(i = 0; i < desc->event_count; i++)\n");
		fprintf(f, "\t{\n");
		fprintf(f, "\t\tfor(j = 0; j < C_APC_COUNT; j++)\n");
		fprintf(f, "\t\t{\n");
		fprintf(f, "\t\t\tdesc->events[i].anim[j].points = NULL;\n");
		fprintf(f, "\t\t\tdesc->events[i].anim[j].point_count = 0;\n");
		fprintf(f, "\t\t}\n");
		fprintf(f, "\t}\n");
		for(i = 0; i < desc->event_count; i++)
		{
			fprintf(f, "/* -------------------------------------- */\n");
			
			fprintf(f, "\tsprintf(desc->events[%u].name, \"%s\");\n", i, desc->events[i].name);
			fprintf(f, "\tdesc->events[%u].speed = %f;\n", i, desc->events[i].speed);
			if(desc->events[i].loop)
				fprintf(f, "\tdesc->events[%u].loop = TRUE;\n", i);
			else
				fprintf(f, "\tdesc->events[%u].loop = FALSE;\n", i);

			if(desc->events[i].start < 0 || desc->events[i].start > 1.0)
				desc->events[i].start = 0;
			if(desc->events[i].end < 0 || desc->events[i].end > 1.0)
				desc->events[i].end = 1;
			fprintf(f, "\tdesc->events[%u].start = %f;\n", i, desc->events[i].start);
			fprintf(f, "\tdesc->events[%u].end = %f;\n", i, desc->events[i].end);
			for(j = 0; j < C_APC_COUNT; j++)
			{
			//	fprintf(f, "\tdesc->events[%u].anim[%u].points = NULL;\n", i, j);
			//	fprintf(f, "\tdesc->events[%u].anim[%u].point_count = 0;\n", i, j);
				for(k = 0; k < desc->events[i].anim[j].point_count; k++)
				{
					if(desc->events[i].anim[j].points[k].space == C_CM_BASE)
					{
						fprintf(f, "\tpos[0] = %f * scale;\n", desc->events[i].anim[j].points[k].pos[0]);
						fprintf(f, "\tpos[1] = %f * scale;\n", desc->events[i].anim[j].points[k].pos[1]);
						fprintf(f, "\tpos[2] = %f * scale;\n", desc->events[i].anim[j].points[k].pos[2]);
					}else
					{
						fprintf(f, "\tpos[0] = %f;\n", desc->events[i].anim[j].points[k].pos[0]);
						fprintf(f, "\tpos[1] = %f;\n", desc->events[i].anim[j].points[k].pos[1]);
						fprintf(f, "\tpos[2] = %f;\n", desc->events[i].anim[j].points[k].pos[2]);
					}
					fprintf(f, "\tc_character_desc_key_create(desc, %u, %u, %f, %u, pos);\n", i, j, desc->events[i].anim[j].points[k].time, desc->events[i].anim[j].points[k].space);
				}
			}
			fprintf(f, "\n");
		}
		
	}
	fprintf(f, "}\n");
}
