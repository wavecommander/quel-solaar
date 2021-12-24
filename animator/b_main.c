
/*#include "opengl3.0.h"*/

#include <stdlib.h>

#include "betray.h"
#include <stdio.h>
#include <time.h>

extern boolean b_sdl_system_wrapper_set_display(uint *size_x, uint *size_y, boolean *full_screen);
extern boolean b_glut_system_wrapper_set_display(uint size_x, uint size_y, boolean full_screen);
extern boolean b_win32_system_wrapper_set_display(uint size_x, uint size_y, boolean full_screen);

boolean betray_capture = FALSE;

typedef struct{
	uint		x_size;
	uint		y_size;
	double		fov;
	double		view_far;
	double		view_near;
	boolean	fullscreen;
}GraphicsMode;

struct{
	BInputState		input;
	boolean			input_clear;
	BActionMode		action_mode;
	GraphicsMode	screen_mode;
	void			(*action_func)(BInputState *input, void *user);
	void			*action_func_data;
	uint32			time[2];
	double			delta_time;
}BGlobal;

void betray_reshape_view(uint x_size, uint y_size)
{
	float w, h, fov, aspect;
	BGlobal.screen_mode.x_size = x_size;
	BGlobal.screen_mode.y_size = y_size;
	w = BGlobal.screen_mode.x_size;
	h = BGlobal.screen_mode.y_size;
//	fov = atan(90 / 180 * 3.14 );
	fov = 1;
	glMatrixMode(GL_PROJECTION);
	aspect = w / h;
	glLoadIdentity();
	glFrustum(-fov * 0.005, fov * 0.005, (-fov / aspect) * 0.005, (fov / aspect) * 0.005, 0.005, 10000.0);
	glViewport(0, 0, x_size, y_size);
	glMatrixMode(GL_MODELVIEW);	
}

boolean betray_set_screen_mode(uint x_size, uint y_size, boolean fullscreen)
{
	printf("Coming from Fullscreen %u\n", (uint)fullscreen);
	#ifdef BETRAY_SDL_SYSTEM_WRAPPER
	if(b_sdl_system_wrapper_set_display(&x_size, &y_size, &fullscreen) != TRUE)
		return FALSE;
	#endif 

	#ifdef BETRAY_GLUT_SYSTEM_WRAPPER
	if(b_glut_system_wrapper_set_display(x_size, y_size, fullscreen) != TRUE)
		return FALSE;
	#endif

	#ifdef BETRAY_GLFW_SYSTEM_WRAPPER
	if(b_glfw_system_wrapper_set_display(x_size, y_size, fullscreen) != TRUE)
		return FALSE;
	#endif
/*
	#ifdef BETRAY_WIN32_SYSTEM_WRAPPER
	if(b_win32_init_display(x_size, y_size, fullscreen) != TRUE)
		return FALSE;
	#endif*/

	BGlobal.screen_mode.x_size = x_size;
	BGlobal.screen_mode.y_size = y_size;
	printf("Finaly setting from Fullscreen %u\n", (uint)fullscreen);
	BGlobal.screen_mode.fullscreen = fullscreen;
	betray_reshape_view(x_size, y_size);
	return TRUE;
}

void betray_set_frustum_mode(double view_far, double view_near, double fov)
{
	BGlobal.screen_mode.view_far = view_far;
	BGlobal.screen_mode.view_near = view_near;
	BGlobal.screen_mode.fov = fov;
	betray_reshape_view(BGlobal.screen_mode.x_size, BGlobal.screen_mode.y_size);
}

double betray_get_screen_mode(uint *x_size, uint *y_size, boolean *fullscreen)
{
	if(x_size != NULL)
		*x_size = BGlobal.screen_mode.x_size;
	if(y_size != NULL)
		*y_size = BGlobal.screen_mode.y_size;
	if(fullscreen != NULL)
		*fullscreen = BGlobal.screen_mode.fullscreen;
	return (double)BGlobal.screen_mode.y_size / (double)BGlobal.screen_mode.x_size;
}


void betray_get_frustum_mode(double *view_far, double *view_near, double *fov)
{
	if(view_far != NULL)
		*view_far = BGlobal.screen_mode.view_far;
	if(view_near != NULL)
		*view_near = BGlobal.screen_mode.view_near;
	if(fov != NULL)
		*fov = BGlobal.screen_mode.fov;
}

extern void b_sdl_init_display(uint *size_x, uint *size_y, boolean *full_screen, char *caption);
extern void b_glut_init_display(int argc, char **argv, uint size_x, uint size_y, boolean full_screen, char *caption);
extern boolean b_win32_init_display(uint size_x, uint size_y, boolean full_screen, char *caption);


void betray_init(int argc, char **argv, uint window_size_x, uint window_size_y, boolean window_fullscreen, char *name)
{
	#ifdef BETRAY_SDL_SYSTEM_WRAPPER
	b_sdl_init_display(&window_size_x, &window_size_y, &window_fullscreen, name);
	printf("window_size_x %u window_size_y %u\n", window_size_x, window_size_y);
	#endif 

	#ifdef BETRAY_GLUT_SYSTEM_WRAPPER
	b_glut_init_display(argc, argv, window_size_x, window_size_y, window_fullscreen, name);
	#endif

	#ifdef BETRAY_GLFW_SYSTEM_WRAPPER
	b_glfw_init_display(argc, argv, window_size_x, window_size_y, window_fullscreen, name);
	#endif

	#ifdef BETRAY_WIN32_SYSTEM_WRAPPER

	if(!b_win32_init_display(window_size_x, window_size_y, window_fullscreen, name))
	{
		int *a = NULL;
		a[0] = 0;
	}
	#endif


	betray_get_current_time(&BGlobal.time[0], &BGlobal.time[1]);

	BGlobal.screen_mode.view_near = 0.005;
	BGlobal.screen_mode.view_far = 10000.0;
	BGlobal.screen_mode.fov = 1.0;	
	BGlobal.screen_mode.x_size = window_size_x;
	BGlobal.screen_mode.y_size = window_size_y;
	BGlobal.screen_mode.fullscreen = window_fullscreen;
}

BInputState *betray_get_input_state(void)
{
	return &BGlobal.input;
}

float betray_get_time(void)
{
	return BGlobal.input.time;
}

void betray_set_action_func(void (*action_func)(BInputState *data, void *user_pointer), void *user_pointer)
{
	BGlobal.action_func = action_func;
	BGlobal.action_func_data = user_pointer;
}

extern void sui_draw_3d_line_gl(float start_x, float start_y,  float start_z, float end_x, float end_y, float end_z, float red, float green, float blue, float alpha);


void betray_action(BActionMode mode)
{
	BGlobal.input.mode = mode;
	if(mode == BAM_MAIN)
	{
		if(BGlobal.action_func != NULL)
			BGlobal.action_func(&BGlobal.input, BGlobal.action_func_data);
		return;
	}
	if(mode == BAM_DRAW)
		glPushMatrix();
	if(BGlobal.action_func != NULL)
		BGlobal.action_func(&BGlobal.input, BGlobal.action_func_data);
	if(mode == BAM_DRAW)
	{
		glPopMatrix();
	/*	glPushMatrix();
		if(BGlobal.input.mouse_button[0])
			sui_draw_3d_line_gl(BGlobal.input.pointer_x + 0.1, BGlobal.input.pointer_y + 0.05,  -1, BGlobal.input.pointer_x + 0.15, BGlobal.input.pointer_y + 0.05, -1, 1, 0.5, 0.5, 1);
		if(BGlobal.input.mouse_button[1])
			sui_draw_3d_line_gl(BGlobal.input.pointer_x + 0.1, BGlobal.input.pointer_y + 0.10,  -1, BGlobal.input.pointer_x + 0.15, BGlobal.input.pointer_y + 0.10, -1, 0.5, 1, 0.5, 1);
		if(BGlobal.input.mouse_button[2])
			sui_draw_3d_line_gl(BGlobal.input.pointer_x + 0.1, BGlobal.input.pointer_y + 0.15,  -1, BGlobal.input.pointer_x + 0.15, BGlobal.input.pointer_y + 0.15, -1, 0.5, 0.5, 1, 1);


		
		sui_draw_3d_line_gl(BGlobal.input.pointer_x - 0.1, BGlobal.input.pointer_y,  -1, BGlobal.input.pointer_x + 0.1, BGlobal.input.pointer_y, -1, 0.5, 0.5, 0.5, 1);
		sui_draw_3d_line_gl(BGlobal.input.pointer_x, BGlobal.input.pointer_y - 0.1,  -1, BGlobal.input.pointer_x, BGlobal.input.pointer_y + 0.1, -1, 0.5, 0.5, 0.5, 1);
		glPopMatrix();*/
	}
}

boolean betray_get_key(uint key)
{
	uint i;
	for(i = 0; i < BGlobal.input.event_count; i++)
		if(BGlobal.input.event[i].button == key && BGlobal.input.event[i].state)
			return TRUE;
	return FALSE;
}

void betray_get_key_up_down(boolean *press, boolean *last_press, uint key)
{
	uint i;
	*last_press = *press;
	for(i = 0; i < BGlobal.input.event_count; i++)
	{
		if(BGlobal.input.event[i].button == key)
		{
			*press = BGlobal.input.event[i].state;
		}
	}
}

extern void b_glut_main_loop(void);


static char *type_in_string = NULL;
static uint type_in_alocated = 0;
static int cursor_pos = 0;
static int *cursor_pos_pointer = NULL;
static uint type_in_length = 0;
static void (*type_in_done_func)(void *user, boolean cansle) = NULL;
static void *func_param;

void betray_start_type_in(char *string, uint length, void (*done_func)(void *user, boolean cancel), uint *cursor, void *user_pointer)
{
	type_in_string = string;
	type_in_alocated = length;
	type_in_done_func = done_func;
	func_param = user_pointer;
	if(cursor == NULL)
		cursor_pos_pointer = &cursor_pos;
	else
		cursor_pos_pointer = cursor;
	cursor_pos = 0;
	for(type_in_length = 0; string[type_in_length] != 0; type_in_length++)
		;
/*	for(i = AXIS_BUTTON_VECTOR_1_X; i <= AXIS_BUTTON_VECTOR_2_Z; i++)
		out_going_data.axis_state[i] = 0;*/

}

void betray_end_type_in_mode(boolean cancel)
{
	if(type_in_done_func != NULL)
	{
		char *string;
		void (*func)(void *p, boolean c);
		string = type_in_string;
		func = type_in_done_func;
		if(func != NULL)
			func(func_param, cancel);
	}
	type_in_string = NULL;
	type_in_done_func = NULL;
	if(cursor_pos_pointer != 0)
		*cursor_pos_pointer = 0;
	cursor_pos_pointer = NULL;
}

void betray_insert_character(char character)
{
	int i;
	char temp;
	printf("character %u - %s\n", (uint)character, type_in_string);
	if(type_in_length + 1 == type_in_alocated)
		return;
	for(i = (*cursor_pos_pointer)++; i < type_in_alocated; i++)
	{
		temp = type_in_string[i];
		type_in_string[i] = character;
		character = temp;
	}
	printf("done - %s\n", (uint)character, type_in_string);
	type_in_length++;
}

void betray_delete_character(void)
{
	int i;
	char temp , temp2 = 0;
	if(*cursor_pos_pointer == 0)
		return;
	(*cursor_pos_pointer)--;
	for(i = type_in_alocated ; i > *cursor_pos_pointer; i--)
	{
		temp = type_in_string[i - 1];
		type_in_string[i - 1] = temp2;
		temp2 = temp;
	}
	type_in_length--;
}

void betray_move_cursor(int move)
{
	(*cursor_pos_pointer) += move;
	if(*cursor_pos_pointer < 0)
		*cursor_pos_pointer = 0;

	if((*cursor_pos_pointer) > type_in_length)
		(*cursor_pos_pointer) = type_in_length;
}

static char betray_debug_text[256];

char *betray_debug(void)
{
	if(type_in_string != NULL)
		sprintf(betray_debug_text, "pos %u length %u alloc %u", *cursor_pos_pointer, type_in_length, type_in_alocated);
	else
		betray_debug_text[0] = 0;
	return betray_debug_text;
}

boolean betray_is_type_in(void)
{
	return type_in_string != NULL;
}



#if defined _WIN32

void betray_get_current_time(uint32 *seconds, uint32 *fractions)
{
	static LARGE_INTEGER frequency;
	static boolean init = FALSE;
	LARGE_INTEGER counter;

	if(!init)
	{
		init = TRUE;
		QueryPerformanceFrequency(&frequency);
	}

	QueryPerformanceCounter(&counter);
	if(seconds != NULL)
		*seconds = counter.QuadPart / frequency.QuadPart;
	if(fractions != NULL)
		*fractions = (uint32)((((ULONGLONG) 0xffffffffU) * (counter.QuadPart % frequency.QuadPart)) / frequency.QuadPart);
}

#else

#include <sys/time.h>

void betray_get_current_time(uint32 *seconds, uint32 *fractions)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	if(seconds != NULL)
	    *seconds = tv.tv_sec;
	if(fractions != NULL)
		*fractions = tv.tv_usec * 1E-6 * (double) (uint32)~0;
}

#endif

uint		betray_framebuffer_save_frame_rate = 25;
double		betray_framebuffer_save_delta = 0.0;




double betray_get_delta_time(void)
{
//	return 1.0 / 30.0;
	if(betray_capture)
	{
	/*	if(betray_framebuffer_save_delta + BGlobal.delta_time > 1.0 / (double)betray_framebuffer_save_frame_rate)
			return 1.0 / (double)betray_framebuffer_save_frame_rate - betray_framebuffer_save_delta;
		else
			return BGlobal.delta_time;*/
			return 1.0 / (double)betray_framebuffer_save_frame_rate;
			
	}else
		return BGlobal.delta_time;
}

void betray_time_update(void)
{
	uint32 seconds, fractions;
	betray_get_current_time(&seconds, &fractions);
	BGlobal.delta_time = (double)seconds - (double)BGlobal.time[0] + ((double)fractions - (double)BGlobal.time[1]) / (double)(0xffffffff);
//	printf("Delta = %f FPS = %f\n", BGlobal.delta_time, 1.0 / BGlobal.delta_time);
	BGlobal.time[0] = seconds;
	BGlobal.time[1] = fractions;
}

void betray_save_targa(char *file_name, uint8 *data, uint size_x, uint size_y)
{
	FILE *image;
	char *foot = "TRUEVISION-XFILES.";
	uint i, j;

	if((image = fopen(file_name, "wb")) == NULL)
	{
		printf("Could not create file: %s\n", file_name);
		return;
	}
	fputc(0, image);
	fputc(0, image);
	fputc(2, image); /* uncompressed */
	for(i = 3; i < 12; i++)
		fputc(0, image); /* ignore some stuff */
	fputc(size_x % 256, image);  /* size */
	fputc(size_x / 256, image);
	fputc(size_y % 256, image);
	fputc(size_y / 256, image);
	fputc(24, image); /* 24 bit image */
	fputc(0, image); /* 24 bit image */
	for(i = 0; i < size_x * size_y * 4; i += 4)
	{
		fputc(data[i + 2], image);
		fputc(data[i + 1], image);
		fputc(data[i], image);
	}
	for(i = 0; i < 9; i++)
		fputc(0, image); // ignore some stuff
	for(i = 0; foot[i] != 0; i++)
		fputc(foot[i], image); // ignore some stuff
	fputc(0, image);
	fclose(image);
}

#define GL_STREAM_READ_ARB                0x88E1
#define GL_PIXEL_PACK_BUFFER_ARB          0x88EB
#define GL_READ_ONLY_ARB                  0x88B8
#define GL_STREAM_DRAW_ARB                0x88E0

GLvoid		(APIENTRY *b_glBindBufferARB)(enum target, uint buffer);
GLvoid		(APIENTRY *b_glDeleteBuffersARB)(uint n, const uint *buffers);
GLvoid		(APIENTRY *b_glGenBuffersARB)(uint n, uint *buffers);
GLvoid		(APIENTRY *b_glBufferDataARB)(enum target, uint size, const void *data, enum usage);
GLvoid		(APIENTRY *b_glBufferSubDataARB)(enum target, uint offset, uint size, const void *data);
GLvoid		(APIENTRY *b_glGetBufferSubDataARB)(enum target, uint offset, uint size, void *data);
GLvoid*		(APIENTRY *b_glMapBufferARB)(GLenum, GLenum);
GLboolean	(APIENTRY *b_glUnmapBufferARB)(GLenum);

uint		betray_framebuffer_save_offset_x = 0;
uint		betray_framebuffer_save_offset_y = 0;
uint		betray_framebuffer_save_size_x = 128;
uint		betray_framebuffer_save_size_y = 128;
uint		betray_framebuffer_save_frame_count = 1;
uint		betray_framebuffer_save_pbos[2];
char		**betray_framebuffer_save_buffer = NULL;
uint		betray_framebuffer_save_screen_shot_nr = 0;
uint		betray_framebuffer_save_scene_nr = 0;
uint		betray_framebuffer_save_frame_nr = 0;

void betray_framebuffer_save_settings(uint offset_x, uint offset_y, uint size_x, uint size_y, uint frame_count, uint frame_rate)
{
	static boolean init = FALSE;
	uint i;
	betray_framebuffer_save_offset_x = offset_x;
	betray_framebuffer_save_offset_y = offset_y;
	betray_framebuffer_save_size_x = size_x;
	betray_framebuffer_save_size_y = size_y;

	betray_framebuffer_save_frame_count = frame_count;
	betray_framebuffer_save_frame_rate = frame_rate;

	if(!init)
	{
		FILE *image;
		void *(*b_gl_GetProcAddress)(const char* proc) = NULL;
		char file_name[256];
		b_gl_GetProcAddress = betray_get_gl_proc_address();
		b_glBindBufferARB = b_gl_GetProcAddress("glBindBufferARB");
		b_glDeleteBuffersARB = b_gl_GetProcAddress("glDeleteBuffersARB");
		b_glGenBuffersARB = b_gl_GetProcAddress("glGenBuffersARB");
		b_glBufferDataARB = b_gl_GetProcAddress("glBufferDataARB");
		b_glBufferSubDataARB = b_gl_GetProcAddress("glBufferSubDataARB");
		b_glGetBufferSubDataARB = b_gl_GetProcAddress("glGetBufferSubDataARB");
        b_glMapBufferARB = b_gl_GetProcAddress("glMapBufferARB");
        b_glUnmapBufferARB = b_gl_GetProcAddress("glUnmapBufferARB");
		init = TRUE;


		for(betray_framebuffer_save_screen_shot_nr = 0; betray_framebuffer_save_screen_shot_nr < 9999; betray_framebuffer_save_screen_shot_nr++)
		{
			sprintf(file_name, "screen_shot%.4d.tga", betray_framebuffer_save_screen_shot_nr);
			if((image = fopen(file_name, "rb")) == NULL)
				break;
			else
				fclose(image);
		}
		for(betray_framebuffer_save_scene_nr = 0; betray_framebuffer_save_scene_nr < 9999; betray_framebuffer_save_scene_nr++)
		{
			sprintf(file_name, "./shotts/shot_%u_frame%.4d.tga", betray_framebuffer_save_scene_nr, 0);
			if((image = fopen(file_name, "rb")) == NULL)
				break;
			else
				fclose(image);
		}			
	}else
		free(betray_framebuffer_save_buffer);
	betray_framebuffer_save_buffer = malloc((sizeof *betray_framebuffer_save_buffer) * frame_count);
	
	for(i = 0; i < frame_count; i++)
	{
		betray_framebuffer_save_buffer[i] = malloc((sizeof *betray_framebuffer_save_buffer[i]) * size_x * size_y * 4);
		if(betray_framebuffer_save_buffer[i] == NULL)
			exit(0);
	}
	for(i = 0; i < 2; i++)
	{
		b_glGenBuffersARB(1, &betray_framebuffer_save_pbos[i]);
		b_glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, betray_framebuffer_save_pbos[i]);
		b_glBufferDataARB(GL_PIXEL_PACK_BUFFER_ARB, size_x * size_y * 4, NULL, GL_STREAM_DRAW_ARB);		
	}
}

void betray_framebuffer_save()
{
	static uint screen_number = 0, jump = 0;
	char file_name[128];
	uint i;
/*
	jump++;
	if(jump % (40) != 0)
		return;
	printf("betray_framebuffer_save_pbos %p\n", betray_framebuffer_save_pbos);
*/	if(betray_capture == FALSE || betray_framebuffer_save_pbos == NULL)
		return;
	betray_framebuffer_save_delta += BGlobal.delta_time;
/*
	if(betray_framebuffer_save_delta < 1.0 / (float)betray_framebuffer_save_frame_rate)
		return;*/

	if(betray_framebuffer_save_delta > 2.0 / (float)betray_framebuffer_save_frame_rate)
		betray_framebuffer_save_delta = 0.0;
	else
		betray_framebuffer_save_delta -= 1.0 / (float)betray_framebuffer_save_frame_rate;
	glReadBuffer(GL_FRONT);
	b_glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, betray_framebuffer_save_pbos[screen_number % 2]);
	glReadPixels(betray_framebuffer_save_offset_x, betray_framebuffer_save_offset_y,
					betray_framebuffer_save_size_x, betray_framebuffer_save_size_y, GL_RGBA, GL_UNSIGNED_BYTE, 0);

//	betray_framebuffer_save_frame_nr = screen_number;

//	b_glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, betray_framebuffer_save_pbos[(screen_number + 1) % 2]);
	b_glGetBufferSubDataARB(GL_PIXEL_PACK_BUFFER_ARB, 0, betray_framebuffer_save_size_x * betray_framebuffer_save_size_y * 4, betray_framebuffer_save_buffer[(screen_number)]);
	screen_number++;

	if(screen_number == betray_framebuffer_save_frame_count)
	{		
		printf("betray_framebuffer_save_frame_count = %u\n", betray_framebuffer_save_frame_count);
		for(i = 0; i < betray_framebuffer_save_frame_count; i++)
		{
			if(betray_framebuffer_save_frame_count == 1)	
				sprintf(file_name, "betray_screen_shot%.4d.tga", betray_framebuffer_save_screen_shot_nr);
			else
				sprintf(file_name, "./shotts/shot_%u_frame%.4d.tga", betray_framebuffer_save_scene_nr, i);
			printf("saving file = %s - %u %u\n", file_name, betray_framebuffer_save_size_x, betray_framebuffer_save_size_y);
			betray_save_targa(file_name, betray_framebuffer_save_buffer[i], betray_framebuffer_save_size_x, betray_framebuffer_save_size_y);
		}
		betray_capture = FALSE;
		screen_number = 0;
		betray_framebuffer_save_screen_shot_nr++;
		betray_framebuffer_save_scene_nr++;
	}
}
