#include <stdio.h>
#include <stdlib.h>

#include "seduce.h"
#include "s_text.h"

//extern float sui_text_pos(float size, float spacing, const char *text, float left, float spaces, uint end);


void sui_draw_text(float pos_x, float pos_y, float size, float spacing, const char *text, float red, float green, float blue, float alpha)
{
	if(text == NULL || *text == '\0')
		return;
	glPushMatrix();
	glTranslatef(pos_x, pos_y, 0);
	glColor4f(red / 8.0, green / 8.0, blue / 8.0, alpha / 8.0);
	sui_text_line_draw(size, spacing, text, -1, 8);
//	glColor4f(red / 1.0, green / 1.0, blue / 1.0, alpha / 1.0);
//	sui_text_line_draw(size, spacing, text, -1, 1);
	glPopMatrix();
}

boolean sw_text_button(BInputState *input, float pos_x, float pos_y, float center, float size, float spacing, const char *text, float red, float green, float blue, float alpha)
{
	if(input->mode == BAM_DRAW)
	{
		if(center > 0.0001)
			center *= sui_text_line_length(size, spacing, text, -1);
		sui_draw_text(pos_x - center, pos_y, size, spacing, text, red, green, blue, alpha);
	}else if(input->mouse_button[0] == TRUE && input->last_mouse_button[0] == FALSE)
	{
		spacing	= sui_text_line_length(size, spacing, text, -1);
		if(sui_box_click_test(pos_x, pos_y - size, spacing, size * 3) && sui_box_down_click_test(pos_x, pos_y - size, spacing, size * 3))
			return TRUE;
	}
	return FALSE;
}

static uint sui_type_in_cursor = 0;
static char *sui_type_in_text = 0;
static char *sui_type_in_copy = 0;
static char *sui_return_text = 0;
static void (* sui_type_in_done_func)(void *user, char *text); 

void sui_end_type_func(void *user, boolean cancel)
{
	if(cancel == FALSE)
		if(sui_type_in_done_func != NULL)
			sui_type_in_done_func(user, sui_type_in_copy);
	sui_return_text = sui_type_in_text;
	if(sui_type_in_done_func != NULL)
	{
		free(sui_type_in_copy);
		sui_type_in_copy = NULL;
		sui_type_in_done_func = NULL;
	}
	sui_type_in_text = NULL;
}

/*
boolean sui_type_in(BInputState *input, float pos_x, float pos_y, float length, float size, char *text, uint buffer_size, void (*done_func)(void *user, char *text), void* user, float red, float green, float blue, float alpha)
{
	int i;
	float pos;
	if(input->mode == BAM_DRAW)
	{
		char *t;
		if(sui_type_in_text == text && sui_type_in_done_func != NULL)
			t = sui_type_in_copy;
		else
			t = text;

		sui_draw_text(pos_x, pos_y, size, SUI_T_SPACE, t, red, green, blue, alpha);
		if(sui_type_in_text == text)
		{
			pos = pos_x;
			for(i = 0; i < sui_type_in_cursor && t[i] != 0; i++)
				pos += (sui_get_letter_size(t[i]) / sui_get_letter_size(97) + SUI_T_SPACE) * size;
			sui_draw_text(pos + SUI_T_SPACE * 0.5 * size, pos_y, size, SUI_T_SPACE, "|", red, green, blue, alpha);
		}
	}
	else
	{
		if(input->mouse_button[0] == TRUE && input->last_mouse_button[0] == FALSE)
		{
			if(sui_box_click_test(pos_x, pos_y - size, length, size * 3.0))
			{
				pos = pos_x;
				sui_type_in_text = text;
				sui_type_in_done_func = done_func;
				if(done_func != NULL)
				{
					sui_type_in_copy = malloc((sizeof *sui_type_in_copy) * buffer_size);
					for(i = 0; i < buffer_size; i++)
						sui_type_in_copy[i] = text[i];
					betray_start_type_in(sui_type_in_copy, buffer_size, sui_end_type_func, &sui_type_in_cursor, user);
				}else
				{
					betray_start_type_in(text, buffer_size, sui_end_type_func, &sui_type_in_cursor, user);
					sui_type_in_copy = text;
				}
				for(i = 0; text[i] != 0 && i < buffer_size && pos < input->pointer_x; i++);
					pos = sui_get_letter_size(text[i]) * size;
				sui_type_in_cursor = i;
			}
		}
		if(sui_return_text == text)
		{
			sui_return_text = NULL;
			return TRUE;
		}
	}
	return FALSE;
}
*/
boolean sui_type_in(BInputState *input, float pos_x, float pos_y, float length, float size, char *text, uint buffer_size, void (*done_func)(void *user, char *text), void *user, float red, float green, float blue, float alpha)
{
	static uint scroll_start = 0, select_start = 0, select_end = 0;
	char *t;
	uint i;

	if(sui_type_in_text == text && done_func != NULL)
		t = sui_type_in_copy;
	else
		t = text;

	if(input->mode == BAM_DRAW)
	{
		sui_draw_text(pos_x, pos_y, size, SUI_T_SPACE, t, red, green, blue, alpha);
		if(sui_type_in_text == text)
			sui_text_line_edit_draw(pos_x, pos_y, size, SUI_T_SPACE, length, t, &scroll_start, select_start, select_end);
	}else
	{
		if(input->mouse_button[0] == TRUE && input->last_mouse_button[0] == FALSE)
		{
			if(sui_type_in_text != text && sui_box_click_test(pos_x, pos_y - size, length, size * 3.0))
			{
				scroll_start = 0;
				select_start = 0;
				select_end = 0;
				sui_type_in_text = text;
				sui_type_in_done_func = done_func;
				if(done_func != NULL)
				{
					t = sui_type_in_copy = malloc((sizeof *sui_type_in_copy) * buffer_size);
					for(i = 0; i < buffer_size; i++)
						sui_type_in_copy[i] = text[i];
				}else
					sui_type_in_copy = NULL;
			}
		}
		if(sui_type_in_text == text)
			sui_text_line_edit_mouse(size, SUI_T_SPACE, length, t, &scroll_start, &select_start, &select_end, input->mouse_button[0], input->last_mouse_button[0], input->pointer_x - pos_x);

		if(sui_type_in_text == text)
		{
			for(i = 0; i < input->event_count; i++)
			{
				if(input->event[i].state == TRUE)
				{
					if(input->event[i].button == B_BUTTON_FORWARD)
						sui_text_edit_forward(t, &select_start, &select_end);
					else if(input->event[i].button == B_BUTTON_BACK)
						sui_text_edit_back(t, &select_start, &select_end);
					else if(input->event[i].button == B_BUTTON_DELETE)
						sui_text_edit_delete(t, buffer_size, &select_start, &select_end);
					else if(input->event[i].button == B_BUTTON_END)
						sui_text_edit_end(t, buffer_size, &select_start, &select_end);
					else if(input->event[i].button == B_BUTTON_HOME)
						sui_text_edit_home(t, buffer_size, &select_start, &select_end);
					else if(input->event[i].button == B_BUTTON_BACKSPACE)
						sui_text_edit_backspace(t, buffer_size, &select_start, &select_end);
					else if(input->event[i].button == B_BUTTON_ENTER) 
					{
						if(done_func != NULL)
						{
							done_func(user, t);
							free(sui_type_in_copy);
						}
						sui_type_in_text = NULL;
					}
					else if(input->event[i].character > 31 && input->event[i].character < 128)
						sui_text_edit_insert_character(t, buffer_size, &select_start, &select_end, input->event[i].character);
				}
			}
		}
		if(sui_type_in_text == text)
			return TRUE;
	}
	return FALSE;
}

sui_scroll_start = 0;
sui_select_start = 0;
sui_select_end = 0;

void sui_type_in_always_insert(char *text, uint buffer_size, char *insert)
{
	uint i;
	printf("A sui_select_start %u sui_select_end %u\n", sui_select_start, sui_select_end);
	for(i = 0; insert[i] != 0; i++)
		sui_text_edit_insert_character(text, buffer_size, &sui_select_start, &sui_select_end, insert[i]);
	printf("B sui_select_start %u sui_select_end %u\n", sui_select_start, sui_select_end);
}

boolean sui_type_in_always(BInputState *input, float pos_x, float pos_y, float length, float size, char *text, uint buffer_size, float red, float green, float blue, float alpha)
{
//	static uint sui_scroll_start = 0, sui_select_start = 0, sui_select_end = 0;
	uint i;

	if(input->mode == BAM_DRAW)
	{
		sui_draw_text(pos_x, pos_y, size, SUI_T_SPACE, text, red, green, blue, alpha);
		sui_text_line_edit_draw(pos_x, pos_y, size, SUI_T_SPACE, length, text, &sui_scroll_start, sui_select_start, sui_select_end);
	}else
	{

		if(sui_box_click_test(pos_x, pos_y - size, length, size * 3.0) && input->mouse_button[0] == TRUE)
		{
			if(input->last_mouse_button[0] == FALSE)
			{
				sui_scroll_start = 0;
				sui_select_start = 0;
				sui_select_end = 0;
			}
			sui_text_line_edit_mouse(size, SUI_T_SPACE, length, text, &sui_scroll_start, &sui_select_start, &sui_select_end, input->mouse_button[0], input->last_mouse_button[0], input->pointer_x - pos_x);
		}
		for(i = 0; i < input->event_count; i++)
		{
			if(input->event[i].state == TRUE)
			{
			//	printf("input->event[i].button = %u %u\n", input->event[i].button, input->event[i].character);
				if(input->event[i].button == B_BUTTON_FORWARD)
					sui_text_edit_forward(text, &sui_select_start, &sui_select_end);
				else if(input->event[i].button == B_BUTTON_BACK)
					sui_text_edit_back(text, &sui_select_start, &sui_select_end);
				else if(input->event[i].button == B_BUTTON_DELETE)
					sui_text_edit_delete(text, buffer_size, &sui_select_start, &sui_select_end);
				else if(input->event[i].button == B_BUTTON_END)
					sui_text_edit_end(text, buffer_size, &sui_select_start, &sui_select_end);
				else if(input->event[i].button == B_BUTTON_HOME)
					sui_text_edit_home(text, buffer_size, &sui_select_start, &sui_select_end);
				else if(input->event[i].button == B_BUTTON_BACKSPACE)
					sui_text_edit_backspace(text, buffer_size, &sui_select_start, &sui_select_end);
				else if(input->event[i].button == B_BUTTON_ENTER)
				{
					sui_scroll_start = 0;
					sui_select_start = 0;
					sui_select_end = 0;
					return TRUE;
				}
				else if(input->event[i].character > 31 && input->event[i].character < 128)
					sui_text_edit_insert_character(text, buffer_size, &sui_select_start, &sui_select_end, input->event[i].character);
			}
		}

	}
	return FALSE;
}

#define SUI_ILLEGAL_NUMBER         1.7976931348623158e+308 /* max value */

static char *sui_type_in_number_text = NULL;
static double sui_type_in_number_output = SUI_ILLEGAL_NUMBER;
static void *sui_type_in_number_id = NULL;

void sui_end_type_number_func(void *user, boolean cancel)
{
	if(cancel != TRUE)
		sscanf(sui_type_in_number_text, "%lf", &sui_type_in_number_output);
	free(sui_type_in_number_text);
	sui_type_in_number_text = NULL;
}


boolean sui_type_number_double(BInputState *input, float pos_x, float pos_y, float center, float length, float size, double *number, void *id, float red, float green, float blue, float alpha)
{
	static uint scroll_start = 0, select_start = 0, select_end = 0;
	uint i;

	if(input->mode == BAM_DRAW)
	{
		if(sui_type_in_number_id == id)
		{
			sui_draw_text(pos_x, pos_y, size, SUI_T_SPACE, sui_type_in_copy, red, green, blue, alpha);
			sui_text_line_edit_draw(pos_x, pos_y, size, SUI_T_SPACE, length, sui_type_in_copy, &scroll_start, select_start, select_end);
		}else
		{
			char nr[64];
			sprintf(nr, "%.4f", *number);
			sui_draw_text(pos_x, pos_y, size, SUI_T_SPACE, nr, red, green, blue, alpha);
		}
	}else
	{
		if(input->mouse_button[0] == TRUE && input->last_mouse_button[0] == FALSE)
		{
			if(sui_type_in_number_id != id && sui_box_click_test(pos_x, pos_y - size, length, size * 3.0))
			{
				scroll_start = 0;
				select_start = 0;
				select_end = 0;
				sui_type_in_number_id = id;
				if(sui_type_in_copy == NULL)
					sui_type_in_copy = malloc((sizeof *sui_type_in_copy) * 64);
				sprintf(sui_type_in_copy, "%.4f", *number);
			}
		}
		if(sui_type_in_number_id == id)
			sui_text_line_edit_mouse(size, SUI_T_SPACE, length, sui_type_in_copy, &scroll_start, &select_start, &select_end, input->mouse_button[0], input->last_mouse_button[0], input->pointer_x - pos_x);

		if(sui_type_in_number_id == id)
		{
			for(i = 0; i < input->event_count; i++)
			{
				if(input->event[i].state == TRUE)
				{
					printf("input->event[i].button = %u %u\n", input->event[i].button, input->event[i].character);
					if(input->event[i].button == B_BUTTON_FORWARD)
						sui_text_edit_forward(sui_type_in_copy, &select_start, &select_end);
					else if(input->event[i].button == B_BUTTON_BACK)
						sui_text_edit_back(sui_type_in_copy, &select_start, &select_end);
					else if(input->event[i].button == B_BUTTON_DELETE)
						sui_text_edit_delete(sui_type_in_copy, 64, &select_start, &select_end);
					else if(input->event[i].button == B_BUTTON_END)
						sui_text_edit_end(sui_type_in_copy, 64, &select_start, &select_end);
					else if(input->event[i].button == B_BUTTON_HOME)
						sui_text_edit_home(sui_type_in_copy, 64, &select_start, &select_end);
					else if(input->event[i].button == B_BUTTON_BACKSPACE)
						sui_text_edit_backspace(sui_type_in_copy, 64, &select_start, &select_end);
					else if(input->event[i].button == B_BUTTON_ENTER) 
					{
						sscanf(sui_type_in_copy, "%lf", number);
						sui_type_in_number_id = NULL;
						return TRUE;
					}
					else if(input->event[i].character >= 48 && input->event[i].character <= 57)
						sui_text_edit_insert_character(sui_type_in_copy, 64, &select_start, &select_end, input->event[i].character);
					else if(input->event[i].character == 44 || input->event[i].character == 46)
						sui_text_edit_insert_character(sui_type_in_copy, 64, &select_start, &select_end, 46);
					else if(input->event[i].character == 45)
						sui_text_edit_insert_character(sui_type_in_copy, 64, &select_start, &select_end, 45);
				}
			}
		}
	}
	return FALSE;
}
/*
boolean sui_type_number_double(BInputState *input, float pos_x, float pos_y, float center, float length, float size, double *number, void *id, float red, float green, float blue, float alpha)
{
	int i;
	float pos;
	if(input->mode == BAM_DRAW)
	{
		if(sui_type_in_number_text != NULL && sui_type_in_number_id == id)
		{
			pos_x -= sui_text_line_length(size, SUI_T_SPACE, sui_type_in_number_text, -1) * center;
			sui_draw_text(pos_x, pos_y, size, SUI_T_SPACE, sui_type_in_number_text, red, green, blue, alpha);
			pos = pos_x;
			for(i = 0; i < sui_type_in_cursor && sui_type_in_number_text[i] != 0; i++)
				pos += (sui_get_letter_size(sui_type_in_number_text[i]) / sui_get_letter_size(97) + SUI_T_SPACE) * size;
			sui_draw_text(pos + SUI_T_SPACE * 0.5 * size, pos_y, size, SUI_T_SPACE, "|", red, green, blue, alpha);
		}else
		{
			char nr[64];
			sprintf(nr, "%.3f", *number);
			pos_x -= sui_text_line_length(size, SUI_T_SPACE, nr, -1) * center;
			sui_draw_text(pos_x, pos_y, size, SUI_T_SPACE, nr, red, green, blue, alpha);
		}
	}
	else
	{
		if(input->mouse_button[0] == TRUE && input->last_mouse_button[0] == FALSE)
		{
			if(sui_box_click_test(pos_x - center * length, pos_y - size, length, size * 3.0))
			{
				pos = pos_x;
				sui_type_in_number_text = malloc((sizeof *sui_type_in_number_text) * 64);
				sprintf(sui_type_in_number_text, "%.3f", *number);
				betray_start_type_in(sui_type_in_number_text, 64, sui_end_type_number_func, &sui_type_in_cursor, NULL);
				sui_type_in_number_id = id;
				sui_type_in_number_output = SUI_ILLEGAL_NUMBER;
				for(i = 0; sui_type_in_number_text[i] != 0 && i < 64 && pos < input->pointer_x; i++);
					pos = sui_get_letter_size(sui_type_in_number_text[i]) * size;
				sui_type_in_cursor = i;
			}
		}
		if(sui_type_in_number_id == id && sui_type_in_number_output != SUI_ILLEGAL_NUMBER)
		{
			sui_type_in_number_id = NULL;
			*number = sui_type_in_number_output;
			return TRUE;
		}
	}
	return FALSE;
}
*/
boolean sui_type_number_uint(BInputState *input, float pos_x, float pos_y, float center, float length, float size, uint32 *number, void *id, float red, float green, float blue, float alpha)

{
	static uint scroll_start = 0, select_start = 0, select_end = 0;
	uint i;

	if(input->mode == BAM_DRAW)
	{
		if(sui_type_in_number_id == id)
		{
			sui_draw_text(pos_x, pos_y, size, SUI_T_SPACE, sui_type_in_copy, red, green, blue, alpha);
			sui_text_line_edit_draw(pos_x, pos_y, size, SUI_T_SPACE, length, sui_type_in_copy, &scroll_start, select_start, select_end);
		}else
		{
			char nr[64];
			sprintf(nr, "%u", *number);
			sui_draw_text(pos_x, pos_y, size, SUI_T_SPACE, nr, red, green, blue, alpha);
		}
	}else
	{
		if(input->mouse_button[0] == TRUE && input->last_mouse_button[0] == FALSE)
		{
			if(sui_type_in_number_id != id && sui_box_click_test(pos_x, pos_y - size, length, size * 3.0))
			{
				scroll_start = 0;
				select_start = 0;
				select_end = 0;
				sui_type_in_number_id = id;
				if(sui_type_in_copy == NULL)
					sui_type_in_copy = malloc((sizeof *sui_type_in_copy) * 64);
				sprintf(sui_type_in_copy, "%u", *number);
			}
		}
		if(sui_type_in_number_id == id)
			sui_text_line_edit_mouse(size, SUI_T_SPACE, length, sui_type_in_copy, &scroll_start, &select_start, &select_end, input->mouse_button[0], input->last_mouse_button[0], input->pointer_x - pos_x);

		if(sui_type_in_number_id == id)
		{
			for(i = 0; i < input->event_count; i++)
			{
				if(input->event[i].state == TRUE)
				{
					if(input->event[i].button == B_BUTTON_FORWARD)
						sui_text_edit_forward(sui_type_in_copy, &select_start, &select_end);
					else if(input->event[i].button == B_BUTTON_BACK)
						sui_text_edit_back(sui_type_in_copy, &select_start, &select_end);
					else if(input->event[i].button == B_BUTTON_DELETE)
						sui_text_edit_delete(sui_type_in_copy, 64, &select_start, &select_end);
					else if(input->event[i].button == B_BUTTON_END)
						sui_text_edit_end(sui_type_in_copy, 64, &select_start, &select_end);
					else if(input->event[i].button == B_BUTTON_HOME)
						sui_text_edit_home(sui_type_in_copy, 64, &select_start, &select_end);
					else if(input->event[i].button == B_BUTTON_BACKSPACE)
						sui_text_edit_backspace(sui_type_in_copy, 64, &select_start, &select_end);
					else if(input->event[i].button == B_BUTTON_ENTER) 
					{
						sscanf(sui_type_in_copy, "%u", number);
						sui_type_in_number_id = NULL;
						return TRUE;
					}
					else if(input->event[i].character >= 48 && input->event[i].character <= 57)
						sui_text_edit_insert_character(sui_type_in_copy, 64, &select_start, &select_end, input->event[i].character);
				}
			}
		}
	}
	return FALSE;
}

/*
boolean sui_type_number_uint(BInputState *input, float pos_x, float pos_y, float center, float length, float size, uint32 *number, void *id, float red, float green, float blue)
{
	int i;
	float pos;
	if(input->mode == BAM_DRAW)
	{
		if(sui_type_in_number_text != NULL && sui_type_in_number_id == id)
		{
			pos_x -= sui_text_line_length(size, SUI_T_SPACE, sui_type_in_number_text, -1) * center;
			sui_draw_text(pos_x, pos_y, size, SUI_T_SPACE, sui_type_in_number_text, red, green, blue, 1.0);
			pos = pos_x;
			for(i = 0; i < sui_type_in_cursor && sui_type_in_number_text[i] != 0; i++)
				pos += (sui_get_letter_size(sui_type_in_number_text[i]) / sui_get_letter_size(97) + SUI_T_SPACE) * size;
			sui_draw_text(pos + SUI_T_SPACE * 0.5 * size, pos_y, size, SUI_T_SPACE, "|", red, green, blue, 1.0);
		}else
		{
			char nr[64];
			sprintf(nr, "%u", *number);
			pos_x -= sui_text_line_length(size, SUI_T_SPACE, nr, -1) * center;
			sui_draw_text(pos_x, pos_y, size, SUI_T_SPACE, nr, red, green, blue, 1.0);
		}
	}
	else
	{
		if(input->mouse_button[0] == TRUE && input->last_mouse_button[0] == FALSE)
		{
			if(sui_box_click_test(pos_x - center * length, pos_y - size, length, size * 3.0))
			{
				pos = pos_x;
				sui_type_in_number_text = malloc((sizeof *sui_type_in_number_text) * 64);
				sprintf(sui_type_in_number_text, "%u", *number);
				betray_start_type_in(sui_type_in_number_text, 64, sui_end_type_number_func, &sui_type_in_cursor, NULL);
				sui_type_in_number_id = id;
				sui_type_in_number_output = SUI_ILLEGAL_NUMBER;
				for(i = 0; sui_type_in_number_text[i] != 0 && i < 64 && pos < input->pointer_x; i++);
					pos = sui_get_letter_size(sui_type_in_number_text[i]) * size;
				sui_type_in_cursor = i;
			}
		}
		if(sui_type_in_number_id == id && sui_type_in_number_output != SUI_ILLEGAL_NUMBER)
		{
			sui_type_in_number_id = NULL;
			*number = (uint32)sui_type_in_number_output;
			return TRUE;
		}
	}
	return FALSE;
}*/

