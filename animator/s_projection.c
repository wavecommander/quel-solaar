#include <math.h>
#include "seduce.h"

extern void clear_matrix(double *matrix);
/*
void point_threw_matrix(float *matrix, float *vertex)
*/
extern void point_threw_matrix4(double *matrix, double *pos_x, double *pos_y, double *pos_z, double *pos_w);
extern void point_threw_matrix3(double *matrix, double *pos_x, double *pos_y, double *pos_z);

extern void matrix_multiply(const double *a,const  double *b, double *output);

extern void matrix_rotate_x(double *matrix, double degree);
extern void matrix_rotate_y(double *matrix, double degree);
extern void matrix_rotate_z(double *matrix, double degree);
extern void transform_matrix(double *matrix, double x, double y, double z);
extern void scale_matrix(double *matrix, double x, double y, double z);
extern void negate_matrix(double *matrix);
extern void reverse_matrix(double *matrix);
extern void matrix_print(const double *matrix);


SViewData sui_default_view;

void sui_view_init(SViewData *v)
{
	if(v == NULL)
		v = &sui_default_view;
	v->pitch = 0;
	v->yaw = 0;
	v->pitch_target = 0;
	v->yaw_target = 0;
	v->position[0] = 0;
	v->position[1] = 0;
	v->position[2] = 0;
	v->target[0] = 0;
	v->target[1] = 0;
	v->target[2] = 0;
	v->distance = 10;
	v->distance_target = 10;
	v->view_axis = 3;
	v->speed = 0.8;
    v->grid_size = 1000;
	v->axis = 3;
}

//(A * f + B * (1.0 - f)) * f2 + ((A * B) * f + ((1 - A) * (1 - B)) * f) * (1.0 - f2)

void sui_view_change_start(SViewData *v, BInputState *input)
{
	if(v == NULL)
		v = &sui_default_view;
	v->grab[0] = input->pointer_x;
	v->grab[1] = input->pointer_y;
	v->speed = 0.8;
}

void sui_view_center_set(SViewData *v, double position_x, double position_y, double position_z)
{
	if(v == NULL)
		v = &sui_default_view;
	v->speed = 0.95;
	v->target[0] = position_x;
	v->target[1] = position_y;
	v->target[2] = position_z;
}


void sui_view_direction_set(SViewData *v, double normal_x, double normal_y, double normal_z)
{
	double f;
	if(v == NULL)
		v = &sui_default_view;
	f = 360 * atan2(-normal_x, normal_z) / PI * 2.0;
	if(f > 0 &&  v->yaw < f - 180.0)
		v->yaw += 360;
	if(f < 0 &&  v->yaw > f + 180.0)
		v->yaw -= 360;
	v->yaw_target = f;
	f = (normal_x * normal_x) + (normal_y * normal_y) + (normal_z * normal_z);
	if(0.0 != f)
		v->pitch_target = 360.0 * atan(normal_y / sqrt(f)) / (PI * 2.0);
}

void sui_view_distance_camera_set(SViewData *v, double distance)
{
	double f;
	if(v == NULL)
		v = &sui_default_view;
	v->distance_target = distance;
}

void sui_view_change(SViewData *v, BInputState *input)
{
	float a, b;
	if(v == NULL)
		v = &sui_default_view;
	if(input->mode == BAM_EVENT)
	{
		if(input->last_mouse_button[1] == TRUE)
		{
			if(input->mouse_button[2] == TRUE && input->mouse_button[0] == TRUE)
			{
				sui_view_init(v);
			}else if(input->mouse_button[2] == TRUE)
			{
				v->position[0] += v->model[0] * v->distance * 2 * (v->grab[0] - input->pointer_x);
				v->position[1] += v->model[4] * v->distance * 2 * (v->grab[0] - input->pointer_x);
				v->position[2] += v->model[8] * v->distance * 2 * (v->grab[0] - input->pointer_x);

				v->position[0] += v->model[1] * v->distance * 2 * (v->grab[1] - input->pointer_y);
				v->position[1] += v->model[5] * v->distance * 2 * (v->grab[1] - input->pointer_y);
				v->position[2] += v->model[9] * v->distance * 2 * (v->grab[1] - input->pointer_y);
				v->target[0] = v->position[0];
				v->target[1] = v->position[1];
				v->target[2] = v->position[2];
			}else if(input->mouse_button[0] == TRUE)
			{
				a = v->distance * (1 + (input->delta_pointer_x - input->delta_pointer_y) * 8);
				if(a > 90000)
					a = 90000;
				if(a < 0.01)
					a = 0.01;
				v->distance_target = a;
			}else
			{
				a = v->yaw_target + (input->pointer_x - v->grab[0]) * 100; 
				b = v->pitch_target - (input->pointer_y - v->grab[1]) * 100;
				if(b > 90)
					b = 90;
				if(b < -90)
					b = -90;
		//		ProjectionData.pitch = b;
				v->pitch_target = b;
		//		ProjectionData.yaw = a;
				v->yaw_target = a;
				if(v->yaw > 180 && v->yaw_target > 180)
				{
					v->yaw -= 360;
					v->yaw_target -= 360;
				}else if(v->yaw < -180 && v->yaw_target < -180)
				{
					v->yaw += 360;
					v->yaw_target += 360;
				}
			}
		}
		v->grab[0] = input->pointer_x;
		v->grab[1] = input->pointer_y;

	/*	a = sqrt(ProjectionData.model[2] * ProjectionData.model[2] + ProjectionData.model[6] * ProjectionData.model[6] + ProjectionData.model[10] * ProjectionData.model[10]);
		if(ProjectionData.model[2] / a > 0.95 || ProjectionData.model[2] / a < -0.95)
			ProjectionData.view_axis = 0;
		else if(ProjectionData.model[6] / a > 0.95 || ProjectionData.model[6] / a < -0.95)
			ProjectionData.view_axis = 1;
		else if(ProjectionData.model[10] / a > 0.95 || ProjectionData.model[10] / a < -0.95)
			ProjectionData.view_axis = 2;
		else*/
			v->view_axis = 3;
	}
}

void sui_view_update(SViewData *v)
{
	double	proj[16];
	if(v == NULL)
		v = &sui_default_view;
	glGetDoublev(GL_PROJECTION_MATRIX, v->matrix);
	glGetDoublev(GL_MODELVIEW_MATRIX, v->model);
}

void sui_view_grid_size_set(SViewData *v, double grid_size)
{
	if(v == NULL)
		v = &sui_default_view;
    v->grid_size = 1.0 / grid_size;
}


void sui_view_center_get(double *center)
{
	center[0] = sui_default_view.position[0];
	center[1] = sui_default_view.position[1];
	center[2] = sui_default_view.position[2];
}

void sui_view_camera_get(SViewData *v, double *camera)
{
	if(v == NULL)
		v = &sui_default_view;
	camera[0] = v->model[2] * v->distance + v->position[0];
	camera[1] = v->model[6] * v->distance + v->position[1];
	camera[2] = v->model[10] * v->distance + v->position[2];
}

double sui_view_distance_camera_get(void)
{
	return sui_default_view.distance;
}

uint sui_view_axis_get(void)
{
	return sui_default_view.axis;
}


void sui_view_model_matrixd(SViewData *v, double *matrix)
{
	uint i;
	if(v == NULL)
		v = &sui_default_view;
	for(i = 0; i < 16; i++)
		matrix[i] = v->model[i];
}

void sui_view_model_matrixf(SViewData *v, float *matrix)
{
	uint i;
	if(v == NULL)
		v = &sui_default_view;
	for(i = 0; i < 16; i++)
		matrix[i] = (float)v->model[i];
}

void sui_view_set(SViewData *v)
{
	static float rotate = 0;
	double cam_pos[3];
	rotate++;
	if(v == NULL)
		v = &sui_default_view;
	v->position[0] = v->target[0] * (1 - v->speed) + v->position[0] * v->speed;
	v->position[1] = v->target[1] * (1 - v->speed) + v->position[1] * v->speed;
	v->position[2] = v->target[2] * (1 - v->speed) + v->position[2] * v->speed;
	cam_pos[0] = v->position[0] + v->model[2] * v->distance;
	cam_pos[1] = v->position[1] + v->model[6] * v->distance;
	cam_pos[2] = v->position[2] + v->model[10] * v->distance;
#ifdef PERSUADE_H
	p_lod_set_view_pos(cam_pos);
#endif
	v->pitch = v->pitch_target * (1 - v->speed) + v->pitch * v->speed;
	v->yaw = v->yaw_target * (1 - v->speed) + v->yaw * v->speed;
	v->distance = v->distance_target * (1 - v->speed) + v->distance * v->speed;
	glTranslated(0, 0, -v->distance);
	glRotated(v->pitch, 1, 0, 0); /* the rotate functions now handels radians too */
	glRotated(v->yaw, 0, 1, 0); /* the rotate functions now handels radians too */
	glTranslated(-v->position[0], -v->position[1], -v->position[2]);
	sui_view_update(v);
}


void sui_view_projectiond(SViewData *v, double *output, double x, double y)
{
	if(v == NULL)
		v = &sui_default_view;
	output[0] = v->model[0] * x * v->distance;
	output[1] = v->model[4] * x * v->distance;
	output[2] = v->model[8] * x * v->distance;
	output[0] += v->model[1] * y * v->distance;
	output[1] += v->model[5] * y * v->distance;
	output[2] += v->model[9] * y * v->distance;
	output[0] += v->position[0];
	output[1] += v->position[1];
	output[2] += v->position[2];
}

void sui_view_projectionf(SViewData *v, float *output, float x, float y)
{
	if(v == NULL)
		v = &sui_default_view;
	output[0] = (float)v->model[0] * x * (float)v->distance;
	output[1] = (float)v->model[4] * x * (float)v->distance;
	output[2] = (float)v->model[8] * x * (float)v->distance;
	output[0] += (float)v->model[1] * y * (float)v->distance;
	output[1] += (float)v->model[5] * y * (float)v->distance;
	output[2] += (float)v->model[9] * y * (float)v->distance;
	output[0] += (float)v->position[0];
	output[1] += (float)v->position[1];
	output[2] += (float)v->position[2];
}


void sui_view_projection_vertexf(SViewData *v, float *output, float *vertex, float x, float y)
{
	double dist, z;
	if(v == NULL)
		v = &sui_default_view;
	z = v->model[2] * (vertex[0] - v->position[0]) + v->model[6] * (vertex[1] - v->position[1]) + v->model[10] * (vertex[2] - v->position[2]);
	dist = (v->distance - z);
	if(dist < 0)
		dist = v->distance;
	output[0] = v->model[0] * x * dist;
	output[1] = v->model[4] * x * dist;
	output[2] = v->model[8] * x * dist;
	output[0] += v->model[1] * y * dist;
	output[1] += v->model[5] * y * dist;
	output[2] += v->model[9] * y * dist;
	output[0] += v->model[2] * z;
	output[1] += v->model[6] * z;
	output[2] += v->model[10] * z;
	output[0] += v->position[0];
	output[1] += v->position[1];
	output[2] += v->position[2];
}

void sui_view_projection_vertexd(SViewData *v, double *output, double *vertex, double x, double y)
{
	double dist, z;
	if(v == NULL)
		v = &sui_default_view;
	z = v->model[2] * (vertex[0] - v->position[0]) + v->model[6] * (vertex[1] - v->position[1]) + v->model[10] * (vertex[2] - v->position[2]);
	dist = (v->distance - z);
	if(dist < 0)
		dist = v->distance;
	output[0] = v->model[0] * x * dist;
	output[1] = v->model[4] * x * dist;
	output[2] = v->model[8] * x * dist;
	output[0] += v->model[1] * y * dist;
	output[1] += v->model[5] * y * dist;
	output[2] += v->model[9] * y * dist;
	output[0] += v->model[2] * z;
	output[1] += v->model[6] * z;
	output[2] += v->model[10] * z;
	output[0] += v->position[0];
	output[1] += v->position[1];
	output[2] += v->position[2];
}


void sui_view_projection_screend(SViewData *v, double *output, double x, double y, double z)
{
	if(v == NULL)
		v = &sui_default_view;
	output[0] = (v->model[0] * x) + (v->model[4] * y) + (v->model[8] * z) + v->model[12];
	output[1] = (v->model[1] * x) + (v->model[5] * y) + (v->model[9] * z) + v->model[13];
	output[2] = (v->model[2] * x) + (v->model[6] * y) + (v->model[10] * z) + v->model[14];
	output[0] = -output[0] / output[2];
	output[1] = -output[1] / output[2];
}


void sui_view_projection_screenf(SViewData *v, float *output, float x, float y, float z)
{
	if(v == NULL)
		v = &sui_default_view;
	output[0] = (v->model[0] * x) + (v->model[4] * y) + (v->model[8] * z) + v->model[12];
	output[1] = (v->model[1] * x) + (v->model[5] * y) + (v->model[9] * z) + v->model[13];
	output[2] = (v->model[2] * x) + (v->model[6] * y) + (v->model[10] * z) + v->model[14];
	output[0] = -output[0] / output[2];
	output[1] = -output[1] / output[2];
}


double sui_view_projection_screen_distanced(SViewData *v, double space_x, double space_y, double space_z, double screen_x, double screen_y)
{
	if(v == NULL)
		v = &sui_default_view;
	point_threw_matrix3(v->model, &space_x, &space_y, &space_z);
	space_x = (space_x / space_z) + screen_x ;
	space_y = (space_y / space_z) + screen_y;
	return space_x * space_x + space_y * space_y;
}

void sui_view_projection_planed(SViewData *v, double *dist, uint axis, double pointer_x, double pointer_y , double depth)
{
	double a[3], b[3], r;
	if(v == NULL)
		v = &sui_default_view;
	a[0] = v->model[0] * v->distance * pointer_x;
	a[1] = v->model[4] * v->distance * pointer_x;
	a[2] = v->model[8] * v->distance * pointer_x;
	a[0] += v->model[1] * v->distance * pointer_y;
	a[1] += v->model[5] * v->distance * pointer_y;
	a[2] += v->model[9] * v->distance * pointer_y;
	a[0] += v->position[0];
	a[1] += v->position[1];
	a[2] += v->position[2];
	b[0] = v->model[2] * v->distance;
	b[1] = v->model[6] * v->distance;
	b[2] = v->model[10] * v->distance;
	b[0] += v->position[0];
	b[1] += v->position[1];
	b[2] += v->position[2];
	r = (b[axis] - depth) / (b[axis] - a[axis]);
	dist[0] = b[0] - (b[0] - a[0]) * r;
	dist[1] = b[1] - (b[1] - a[1]) * r;
	dist[2] = b[2] - (b[2] - a[2]) * r;
	dist[axis] = depth;
}

double sui_view_projection_lined(SViewData *v, double *dist, uint axis, double pointer_x, double pointer_y, double *pos)
{
	double a[3], b[3], r, r2, r3;
	if(v == NULL)
		v = &sui_default_view;
	a[0] = v->model[0] * v->distance * pointer_x;
	a[1] = v->model[4] * v->distance * pointer_x;
	a[2] = v->model[8] * v->distance * pointer_x;
	a[0] += v->model[1] * v->distance * pointer_y;
	a[1] += v->model[5] * v->distance * pointer_y;
	a[2] += v->model[9] * v->distance * pointer_y;
	b[0] = v->model[2] * v->distance;
	b[1] = v->model[6] * v->distance;
	b[2] = v->model[10] * v->distance;
	a[0] -= b[0];
	a[1] -= b[1];
	a[2] -= b[2];
	b[0] += v->position[0] - pos[0];
	b[1] += v->position[1] - pos[1];
	b[2] += v->position[2] - pos[2];
	r = sqrt(b[(axis + 1) % 3] * b[(axis + 1) % 3] + b[(axis + 2) % 3] * b[(axis + 2) % 3]);
	r2 = sqrt(a[(axis + 1) % 3] * a[(axis + 1) % 3] + a[(axis + 2) % 3] * a[(axis + 2) % 3]);
	r3 = b[axis] + (a[axis] * r / r2);
	if(dist != NULL)
	{
		a[0] = pos[0];
		a[1] = pos[1];
		a[2] = pos[2];
		a[axis] += r3;
		point_threw_matrix3(v->model, &a[0], &a[1], &a[2]);
		a[0] = (a[0] / a[2]) + pointer_x;
		a[1] = (a[1] / a[2]) + pointer_y;
		*dist = a[0] * a[0] + a[1] * a[1];
	}
	return r3;
}


void sui_view_projection_line_snap(double *output, uint axis, double direction, double *start, double *snap, SUISnapType snap_type)
{
	output[0] = start[0];
	output[1] = start[1];
	output[2] = start[2];
	if(snap_type == SUI_ST_VERTEX)
	{
		output[axis] = snap[axis];
	}else if(snap_type == SUI_ST_LENGTH)
	{
		if(direction > 0)
			output[axis] += snap[3];
		else
			output[axis] -= snap[3];
	}else if(snap_type == SUI_ST_TANGENT)
	{
		double r, vector[3] = {0, 0, 0};
		vector[axis] = 1;
		
		r = vector[0] * snap[3] + vector[1] * snap[4] + vector[2] * snap[5];
		if(r < 0.1 && r > -0.1)
			;//output[axis] += direction;
		else
		{
			r = (snap[3] * (start[0] - snap[0]) + snap[4] * (start[1] - snap[1]) + snap[5] * (start[2] - snap[2])) / r;
			output[0] = start[0] - r * vector[0];
			output[1] = start[1] - r * vector[1];
			output[2] = start[2] - r * vector[2];
		}
	}
}


void sui_view_projection_vertex_with_axis(SViewData *v, double *output, double *start, double pointer_x, double pointer_y, boolean snap, double *closest, SUISnapType snap_type)
{
	double dist, best_dist, pos;
	//static uint axis; 
	uint i; 
	if(v == NULL)
		v = &sui_default_view;
	if(snap != TRUE)
	{
		v->direction = sui_view_projection_lined(v, &best_dist, 0, pointer_x, pointer_y, start);
		v->axis = 0;
		for(i = 1; i < 3; i++)
		{
			pos = sui_view_projection_lined(v, &dist, i, pointer_x, pointer_y, start);
			if(dist < best_dist)
			{
				v->direction = pos;
				best_dist = dist;
				v->axis = i;
			}
		}
		if(v->view_axis < 3)
		{	
			sui_view_projection_planed(v, output, v->view_axis, pointer_x, pointer_y, start[v->view_axis]);
			return;
		}
		if(best_dist > 0.0001)
		{
			sui_view_projection_vertexd(v, output, start, pointer_x, pointer_y);
			return;
		}
	}
	output[0] = start[0];
	output[1] = start[1];
	output[2] = start[2];
	if(snap != TRUE)
		output[v->axis] += v->direction;
	else
	{
      /*  p_get_projection_line(&best_dist, 0, pointer_x, pointer_y, start);
		if(best_dist < 0.0001)
		{
			output[0] = p_get_projection_line(&best_dist, ProjectionData.axis, pointer_x, pointer_y, start);
			output[ProjectionData.axis] += (double)((int)(output[0] * ProjectionData.grid_size)) / ProjectionData.grid_size;
		}
		else
		{*/
			sui_view_projection_line_snap(output, v->axis, v->direction, start, closest, snap_type);
	/*	}*/
	}
}



/*
void sui_view_projection_vertex_with_axis(SViewData *v, double *output, double *start, double pointer_x, double pointer_y, boolean snap, double *closest)
{
	double dist, best_dist, pos, best_pos = 0.0;
	uint i; 
	if(v == NULL)
		v = &sui_default_view;
	if(snap != TRUE)
	{
		best_pos = sui_view_projection_lined(v, &best_dist, 0, pointer_x, pointer_y, start);
		v->axis = 0;
		for(i = 1; i < 3; i++)
		{
			pos = sui_view_projection_lined(v, &dist, i, pointer_x, pointer_y, start);
			if(dist < best_dist)
			{
				best_pos = pos;
				best_dist = dist;
				v->axis = i;
			}
		}
		if(v->view_axis < 3)
		{	
			sui_view_projection_planed(v, output, v->view_axis, pointer_x, pointer_y, start[v->view_axis]);
			return;
		}
		if(best_dist > 0.0001)
		{
			sui_view_projection_vertexd(v, output, start, pointer_x, pointer_y);
			return;
		}
	}
	output[0] = start[0];
	output[1] = start[1];
	output[2] = start[2];
	if(snap != TRUE)
		output[v->axis] += best_pos;
	else
	{
        best_pos = sui_view_projection_lined(v, &best_dist, v->axis, pointer_x, pointer_y, start);
		if(best_dist < 0.0001)
			output[v->axis] += (double)((int)(best_pos * v->grid_size)) / v->grid_size;
		else
			output[v->axis] = closest[v->axis];
	}
}*/
/*
uint p_find_closest_node(float *dist, double x, double y)
{
	ENode *node;
	double trans[3], pos[3], scale[3], r, closest = 1000000;
	uint32 seconds, fractions, output = -1, count = 0;
	verse_session_get_time(&seconds, &fractions);

	for(node = e_ns_get_node_next(0, 0, V_NT_OBJECT); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_OBJECT))
	{
		e_nso_get_pos_time(node, trans, seconds, fractions);
		sui_view_projection_screend(NULL, pos, trans[0], trans[1], trans[2]);
		e_nso_get_scale(node, scale);

		if(pos[2] < 0)
		{
			scale[0] += scale[1] + scale[2]; 
			pos[0] += x;
			pos[1] += y;
			r = (pos[0] * pos[0] + pos[1] * pos[1]);
			if(r > 0 && r < 0.01 && r / (scale[0] * scale[0]) < closest)
			{
				closest = r;
				output = e_ns_get_node_id(node);
			}
		}
		count++;
	}
//	printf("")
	if(dist != NULL)
		*dist = closest;
	return output;
}*/

boolean sui_view_edge_test(double *a, double *b, double x, double y)
{
	double temp, r;
	x = -x;
	y = -y;
	if(a[2] > 0 || b[2] > 0)
		return FALSE;
	if((a[0] - b[0]) * (x - b[0]) + (a[1] - b[1]) * (y - b[1]) < 0)
		return FALSE;
	if((b[0] - a[0]) * (x - a[0]) + (b[1] - a[1]) * (y - a[1]) < 0)
		return FALSE;
	r = sqrt((b[1] - a[1]) * (b[1] - a[1]) + -(b[0] - a[0]) * -(b[0] - a[0]));
	temp = (x - a[0]) * ((b[1] - a[1]) / r) + (y - a[1]) * (-(b[0] - a[0]) / r);
	if(temp > 0.008 || temp < -0.008 || r < 0.0001)
		return FALSE;
	return TRUE;
}

/*
uint p_find_closest_vertex(double *vertex, uint vertex_count, double min_dist, double *selected_distance, double x, double y)
{
	uint32 vertex_count, i;
	double	*vertex;
	double	pos[3], r;
	udg_get_geometry(&vertex_count, NULL, &vertex, NULL, NULL);

	for(i = 0; i < vertex_count; i++)
	{
		if(vertex[i * 3] != V_REAL64_MAX)
		{
			p_get_projection_screen(pos, vertex[i * 3], vertex[i * 3 + 1], vertex[i * 3 + 2]);
			if(pos[2] < 0)
			{
				pos[0] += x;
				pos[1] += y;
				r = pos[0] * pos[0] + pos[1] * pos[1];
				if(r < *selected_distance)
				{
					*selected_distance = r;
					*selected = i;
					if(r <= *distance && udg_get_select(i) < 0.001)
					{
						*distance = r;
						*closest = i;
					}
				}
			}
		}
	}
}*/