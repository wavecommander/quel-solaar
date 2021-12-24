#if !defined(SEDUCE_H)
#define	SEDUCE_H

/*
This is the new and improved .h file for the "ui tool kit" used in varius verse related applications.
*/

#include "st_types.h"
#include "betray.h"

extern void	sui_init(void);
extern void	sui_text_screen_mode_update(void);
extern void	sui_draw_pointer(float x, float y);

extern boolean	sui_box_click_test(float x_pos, float y_pos, float x_size, float y_size);
extern boolean	sui_box_down_click_test(float x_pos, float y_pos, float x_size, float y_size);


/* GL draw helpers */

extern void sui_draw_gl(uint draw_type, const float *array, uint length, uint dimentions, float red, float green, float blue, float alpha);
extern void sui_draw_2d_line_gl(float start_x, float start_y, float end_x, float end_y, float red, float green, float blue, float alpha);
extern void sui_draw_3d_line_gl(float start_x, float start_y,  float start_z, float end_x, float end_y, float end_z, float red, float green, float blue, float alpha);
extern void sui_draw_elements_gl(uint draw_type, float *array, uint *reference, uint length, uint dimentions, float red, float green, float blue, float alpha);

extern void sui_set_blend_gl(uint source, uint destination);

extern void sui_set_color_array_gl(float *array, uint length, uint channels);
extern void sui_set_normal_array_gl(float *array, uint length);
extern void sui_set_texture2D_array_gl(float *array, uint length, uint dimentions, uint texture);

extern void sui_draw_set_vec2(float *array, uint pos, float a, float b);
extern void sui_draw_set_vec3(float *array, uint pos, float a, float b, float c);
extern void sui_draw_set_vec4(float *array, uint pos, float a, float b, float c, float d);

extern void sui_draw_set_ivec2(uint *array, uint pos, uint a, uint b);
extern void sui_draw_set_ivec3(uint *array, uint pos, uint a, uint b, uint c);
extern void sui_draw_set_ivec4(uint *array, uint pos, uint a, uint b, uint c, uint d);

/* text drawing and handeling */
/*
#define SUI_T_SIZE 0.01
#define SUI_T_SPACE 0.3
*/

#define SUI_T_SIZE 0.0125
#define SUI_T_SPACE 0.3

extern void		sui_draw_letter(uint8 letter, float red, float green, float blue, float alpha);
extern float	sui_get_letter_size(unsigned char letter);

extern void		sui_draw_text(float pos_x, float pos_y, float size, float spacing, const char *text, float red, float green, float blue, float alpha);
//extern float	sui_compute_text_length(float size, float spacing, const char *text);
extern float	sui_text_line_length(float size, float spacing, const char *text, uint end);

extern boolean	sw_text_button(BInputState *input, float pos_x, float pos_y, float center, float size, float spacing, const char *text, float red, float green, float blue, float alpha);

extern boolean	sui_type_number_double(BInputState *input, float pos_x, float pos_y, float center, float length, float size, double *number, void *id, float red, float green, float blue, float alpha);
extern boolean	sui_type_number_uint(BInputState *input, float pos_x, float pos_y, float center, float length, float size, uint32 *number, void *id, float red, float green, float blue, float alpha);

extern boolean	sui_type_in(BInputState *input, float pos_x, float pos_y, float length, float size, char *text, uint buffer_size, void (*done_func)(void *user, char *text), void* user, float red, float green, float blue, float alpha);


/* Setting are automaticaly created if you ask for them, just use a "sui_get_setting..." to name and set a default value, and the value will
 be returned if a different value isnt found in the settings file, use the "sui_set_setting..." to set/change a value*/


extern double	sui_get_setting_double(char *setting, double default_value); /* get double setting */
extern void		sui_set_setting_double(char *setting, double value); /* changed / set double setting */
extern uint		sui_get_setting_int(char *setting, uint default_value); /* get uint setting */
extern void		sui_set_setting_int(char *setting, uint value); /* changed / set uint setting */
extern char		*sui_get_setting_text(const char *setting, const char *default_text); /* get text setting */
extern void		sui_set_setting_text(const char *setting, const char *text); /* changed / set text setting */
extern boolean	sui_test_setting_version(uint *version); /* check if your version of the setting have changed */

extern void		sui_save_settings(const char *file_name); /* saves all settings to a file */
extern void		sui_load_settings(const char *file_name); /* loads all settings from a file */

/* view handeling functionality */

void s_view_2d_set(void);
void s_view_2d_handle(BInputState *input, float *pos, boolean active);

void s_view_2d_set_speed(float speed);
void s_view_2d_set_pos(float x, float y, float z);
void s_view_2d_get_pos(float *pos);

float s_view_2d_get_pos_x(float x);
float s_view_2d_get_pos_y(float y);

float s_view_2d_get_view_x(float x);
float s_view_2d_get_view_y(float y);

/* Creates a shadow edge around a shape*/

extern void sui_create_shadow_edge(float size, uint count, float *shadow, float *color, float *square);

typedef enum{
	PU_T_TOP,
	PU_T_BOTTOM,
	PU_T_ANGLE,
	PU_T_VSLIDER,
	PU_T_HSLIDER,
	PU_T_SQUARE,
	PU_T_ICON
}SUIPUType;

typedef struct{
	SUIPUType	type;
	char		*text;
	union{
		float angle[2];
		struct{
			float pos[2];
			float length;
			double *value;
		}slider;
		struct{
			float square[4];
			void (*draw_func)(float pos_x, float pos_y, float size_x, float size_y, float color);
		}square;
		struct{
			float pos[2];
			float size;
			void (*draw_func)(float pos_x, float pos_y, float color);
		}icon;
	}data;
}SUIPUElement;

extern uint sui_draw_popup(BInputState *input, float pos_x, float pos_y, SUIPUElement *element, uint element_count, uint button, float back_color);
extern void	sui_draw_background_ring(float pos_x, float pos_y, float color);

typedef enum{
	S_VET_SPLIT,
	S_VET_SPLIT_MULTI,
	S_VET_BOOLEAN,
	S_VET_INTEGER,
	S_VET_REAL,
	S_VET_SLIDER,
	S_VET_TEXT,
	S_VET_COLOR,
	S_VET_SELECT,
	S_VET_CUSTOM
}SUIViewElementType;


typedef struct{
	SUIViewElementType type;
	char *text;
	union{
		boolean checkbox; 
		uint	integer;
		double	real;
		double	slider;
		struct{
			char	*text;
			uint	length;
		}text;
		double	color[3];
		struct{
			char	*text[10];
			uint	select;
		}select;
		struct{
			uint	current;
			uint	count;
			boolean	add;
			boolean	del;
		}split_multi;
		struct{
			void (*func)(BInputState *input, void *user, double x_pos, double y_pos, double width, double length);
			void *user;
			double length;
		}custom;
	}param;
}SUIViewElement;

extern boolean sui_draw_setting_view(BInputState *input, double x_pos, double y_pos, double width, double size, SUIViewElement *element, uint element_count, char *title, float back_color);
extern float sui_setting_view_size(SUIViewElement *element);
/* --------------------------------------- */

typedef enum{
	SE_LT_LINE,
	SE_LT_CURVE,
	SE_LT_DRAG
}SELineMode;

typedef enum{
	SE_FT_ICON,
	SE_FT_LINE,
	SE_FT_SQUARE,
}SEFuncType;

typedef struct{
	SELineMode	type;
	float		pos_one[2];
	float		pos_two[2];
}SEditorLine;

typedef struct{
	char		name[64];
	SEditorLine *lines;
	uint		line_allocate;
	uint		line_count;
}SDrawing;

extern void se_editor(BInputState *input, SDrawing *d, float pos_x, float pos_y, float size_x, float size_y, float *color_drawing, float *color_interface,  SELineMode mode);

/* the symbol and font editors built in to seduce */

extern void *sui_symbol_editor_func(BInputState *input, void *user_pointer);
extern void *sui_font_editor_func(BInputState *input, void *user_pointer);


extern void se_save_drawing_binary(void *file, SDrawing *d, uint draw_count);
extern SDrawing *se_load_drawing_binary(void *file, uint *draw_count);

 /* view and projection code */

typedef struct{
	double			matrix[16];
	double			model[16];
	double			pitch;
	double			yaw;
	double			pitch_target;
	double			yaw_target;
	double			target[3];
	double			position[3];
	double			speed;
	double			distance;
	double			distance_target;
    double			grid_size;
	double			direction;
	float			grab[2];
	uint			view_axis;
	uint			axis;
}SViewData;

extern void		sui_view_init(SViewData *v);
extern void		sui_view_change_start(SViewData *v, BInputState *input);
extern void		sui_view_change(SViewData *v, BInputState *input);
extern void		sui_view_update(SViewData *v);
extern void		sui_view_set(SViewData *v);

extern void		sui_view_center_set(SViewData *v, double position_x, double position_y, double position_z);
extern void		sui_view_center_get(double *center);
extern void		sui_view_direction_set(SViewData *v, double normal_x, double normal_y, double normal_z);
extern void		sui_view_distance_camera_set(SViewData *v, double distance);
extern double	sui_view_distance_camera_get(void);

extern void		sui_view_camera_get(SViewData *v, double *camera);
extern void		sui_view_grid_size_set(SViewData *v, double grid_size);

extern void		sui_view_model_matrixd(SViewData *v, double *matrix);
extern void		sui_view_model_matrixf(SViewData *v, float *matrix);



extern void		sui_view_projectiond(SViewData *v, double *output, double x, double y);
extern void		sui_view_projectionf(SViewData *v, float *output, float x, float y);
extern void		sui_view_projection_vertexf(SViewData *v, float *output, float *vertex, float x, float y);
extern void		sui_view_projection_vertexd(SViewData *v, double *output, double *vertex, double x, double y);
extern void		sui_view_projection_screend(SViewData *v, double *output, double x, double y, double z);
extern void		sui_view_projection_screenf(SViewData *v, float *output, float x, float y, float z);
extern double	sui_view_projection_screen_distanced(SViewData *v, double space_x, double space_y, double space_z, double screen_x, double screen_y);
extern void		sui_view_projection_planed(SViewData *v, double *dist, uint axis, double pointer_x, double pointer_y , double depth);
extern double	sui_view_projection_lined(SViewData *v, double *dist, uint axis, double pointer_x, double pointer_y, double *pos);

typedef enum{
	SUI_ST_VERTEX,
	SUI_ST_LENGTH,
	SUI_ST_TANGENT
}SUISnapType;

extern void		sui_view_projection_line_snap(double *output, uint axis, double direction, double *start, double *snap, SUISnapType snap_type);
extern void		sui_view_projection_vertex_with_axis(SViewData *v, double *output, double *start, double pointer_x, double pointer_y, boolean snap, double *closest);
extern boolean	sui_view_edge_test(double *a, double *b, double x, double y);

extern void sui_manipulator_point_draw(float pos_x, float pos_y, float scale, float red, float green, float blue);
extern void sui_manipulator_cross_draw(float pos_x, float pos_y, float red, float green, float blue);

extern boolean	sui_manipulator_point(BInputState *input, SViewData *v, float *pos, void *id, float red, float green, float blue, char *name);
extern boolean	sui_manipulator_normal(BInputState *input, SViewData *v, float *pos, float *normal, void *id, float red, float green, float blue);

//extern boolean	sui_manipulator_point(BInputState *input, SViewData *v, float *pos, void *id);
//extern boolean	sui_manipulator_normal(BInputState *input, SViewData *v, float *pos, float *normal, void *id);
extern boolean	sui_manipulator_radius(BInputState *input, SViewData *v, float *pos, float *radius, void *id);
extern boolean	sui_manipulator_matrix_projection(BInputState *input, SViewData *v, float *pos, float *normal, float *up, float *matrix, boolean scale, boolean aspect_lock, void *id);

#include "s_enough_geo_preview.h"
#endif
