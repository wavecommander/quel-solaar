#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "seduce.h"
#include "s_text.h"

void draw_text_line_test()
{
	/* the sixe and spacing of our text, and the length of our text box */

	float size = 0.0125;
	float spacing = 0.3;
	float length = 0.9; 
	static char text[2000] = {65, 66, 67, 68, 65, 66, 67, 68, 65, 66, 67, 68, 65, 66, 67, 68,0}; /* our text buffer we want to edit */
	static uint select_start = 2; /* the start of our selection */
	static uint select_end = 7; /* the end of our selection */
	static uint scroll_start = 0; /* scroll start of our text*/

	/* temp buffers */

	uint end; /* how long we will draw the text */
	uint i;
	float curser_start; /* the position of our selection start */
	float curser_end; /* the position of our selection end */

	/* input */

	float pointer_x; /* the position of the pointer in relation to the text*/
	boolean mouse_button; /* is the mouse clicked */
	boolean last_mouse_button; /* was the the mouse clicked last frame */
	boolean event = FALSE; /* is a key beeing pressed*/
	static char letter = 0; /* letter we want to input */

	/* load input */

	{
		BInputState *input;
		input = betray_get_input_state();
		pointer_x = input->pointer_x;
		mouse_button = input->mouse_button[0];
		last_mouse_button = input->last_mouse_button[0];
		if(input->event_count > 0 && input->event[0].state)
		{
			letter = input->event[0].button;
			event = TRUE;
		}
	}
	sui_text_line_edit_draw(0, 0, size, spacing, length, text, &scroll_start, select_start, select_end);
	sui_text_line_edit_mouse(size, spacing, length, text, &scroll_start, &select_start, &select_end, mouse_button, last_mouse_button, pointer_x);

	if(event == TRUE)
	{
		if(letter == 19) /* forward key */
		{
			sui_text_edit_forward(text, &select_start, &select_end);
		}
		else if(letter == 20) /* back key */
		{
			sui_text_edit_back(text, &select_start, &select_end);
		}
		else if(letter == 127) /* delete */
		{
			sui_text_edit_delete(text, 2000, &select_start, &select_end);
		}
		else if(letter == 23) /* end */
		{
			sui_text_edit_end(text, 2000, &select_start, &select_end);
		}
		else if(letter == 22) /* home */
		{
			sui_text_edit_home(text, 2000, &select_start, &select_end);
		}
		else if(letter == 8) /* backspace */
		{
			sui_text_edit_backspace(text, 2000, &select_start, &select_end);
		}
		else
		{
			sui_text_edit_insert_character(text, 2000, &select_start, &select_end, letter);
		}
	}

}

void draw_text_box_test()
{
	/* the sixe and spacing of our text, and the length of our text box */

	float size = 0.0125;
	float spacing = 0.3;
	float length = 0.9; 
	float line_spacing = 3.0;
	uint lines = 40;
	static char text[2000] = {65, 66, 67, 68, 32, 66, 67, 68, 65, 66, 67, 32, 65, 66, 67, 68, 
	65, 66, 67, 68, 32, 66, 67, 68, 65, 66, 67, 32, 65, 66, 67, 68,	65, 66, 67, 68, 32, 66, 67, 68, 65, 66, 67, 32, 65, 66, 67, 32, 
	65, 66, 67, 68, 32, 66, 67, 68, 65, 66, 67, 32, 65, 66, 32, 68,	65, 66, 67, 68, 32, 66, 67, 32, 65, 66, 67, 32, 65, 66, 67, 68, 
	65, 66, 67, 68, 32, 66, 67, 68, 65, 66, 67, 32, 65, 66, 67, 68,	65, 66, 67, 68, 32, 32, 67, 68, 65, 66, 67, 32, 65, 32, 67, 68, 
	32, 66, 67, 68, 32, 66, 32, 68, 65, 32, 67, 32, 65, 66, 32, 68,	65, 66, 67, 68, 32, 66, 67, 68, 65, 66, 67, 32, 65, 66, 67, 68, 
	65, 66, 67, 68, 32, 66, 67, 68, 65, 66, 67, 32, 65, 66, 67, 68,	65, 66, 67, 68, 32, 66, 67, 68, 65, 32, 67, 32, 65, 66, 67, 68, 
	65, 66, 67, 68, 32, 66, 67, 68, 32, 66, 67, 32, 65, 66, 67, 68,	65, 66, 67, 68, 32, 66, 67, 68, 65, 66, 67, 32, 65, 66, 67, 68, 
	65, 66, 67, 68, 32, 66, 67, 68, 65, 66, 67, 32, 65, 66, 67, 68,	65, 66, 67, 68, 32, 32, 67, 68, 65, 66, 67, 32, 65, 66, 67, 68, 
	65, 66, 67, 68, 32, 66, 67, 68, 65, 66, 32, 32, 65, 66, 67, 68,	65, 66, 67, 68, 32, 66, 67, 68, 32, 66, 67, 32, 65, 66, 67, 68, 
	65, 66, 67, 68, 32, 66, 67, 68, 65, 66, 67, 32, 65, 66, 67, 68,	65, 66, 23, 68, 32, 66, 67, 68, 65, 66, 67, 32, 65, 66, 67, 68, 
	65, 66, 67, 68, 32, 66, 67, 68, 65, 66, 67, 32, 65, 66, 67, 68,	65, 66, 67, 68, 32, 66, 67, 68, 65, 66, 67, 32, 65, 66, 67, 68, 
	65, 66, 67, 68, 32, 66, 67, 68, 65, 66, 67, 32, 65, 66, 32, 68,	65, 32, 67, 68, 32, 66, 67, 68, 65, 32, 67, 32, 65, 66, 67, 68, 
	65, 66, 67, 68, 32, 66, 67, 68, 32, 32, 67, 32, 65, 66, 67, 68,	65, 66, 67, 68, 32, 66, 67, 68, 65, 66, 67, 32, 65, 66, 67, 68, 
	65, 66, 67, 68, 32, 66, 67, 68, 65, 66, 67, 32, 65, 66, 67, 68,	65, 66, 67, 68, 32, 66, 67, 68, 65, 66, 67, 32, 65, 66, 67, 32, 
	65, 66, 67, 68, 32, 66, 67, 68, 65, 66, 67, 32, 65, 66, 32, 68,	65, 66, 67, 68, 32, 66, 67, 32, 65, 66, 67, 32, 65, 66, 67, 68, 
	65, 66, 67, 68, 32, 66, 67, 68, 65, 66, 67, 32, 65, 66, 67, 68,	65, 66, 67, 68, 32, 32, 67, 68, 65, 66, 67, 32, 65, 32, 67, 68, 
	32, 66, 67, 68, 32, 66, 32, 68, 65, 32, 67, 32, 65, 66, 32, 68,	65, 66, 67, 68, 32, 66, 67, 68, 65, 66, 67, 32, 65, 66, 67, 68, 
	65, 66, 67, 68, 32, 66, 67, 68, 65, 66, 67, 32, 65, 66, 67, 68,	65, 66, 67, 68, 32, 66, 67, 68, 65, 32, 67, 32, 65, 66, 67, 68, 
	65, 66, 67, 68, 32, 66, 67, 68, 32, 66, 67, 32, 65, 66, 67, 68,	65, 66, 67, 68, 32, 66, 67, 68, 65, 66, 67, 32, 65, 66, 67, 68, 
	65, 66, 67, 68, 32, 66, 67, 68, 65, 66, 67, 32, 65, 66, 67, 68,	65, 66, 67, 68, 32, 32, 67, 68, 65, 66, 67, 32, 65, 66, 67, 68, 
	65, 66, 67, 68, 32, 66, 67, 68, 65, 66, 32, 32, 65, 66, 67, 68,	65, 66, 67, 68, 32, 66, 67, 68, 32, 66, 67, 32, 65, 66, 67, 68, 
	65, 66, 67, 68, 32, 66, 67, 68, 65, 66, 67, 32, 65, 66, 67, 68,	65, 66, 23, 68, 32, 66, 67, 68, 65, 66, 67, 32, 65, 66, 67, 68, 
	65, 66, 67, 68, 32, 66, 67, 68, 65, 66, 67, 32, 65, 66, 67, 68,	65, 66, 67, 68, 32, 66, 67, 68, 65, 66, 67, 32, 65, 66, 67, 68, 
	65, 66, 67, 68, 32, 66, 67, 68, 65, 66, 67, 32, 65, 66, 32, 68,	65, 32, 67, 68, 32, 66, 67, 68, 65, 32, 67, 32, 65, 66, 67, 68, 
	65, 66, 67, 68, 32, 66, 67, 68, 32, 32, 67, 32, 65, 66, 67, 68,	65, 66, 67, 68, 32, 66, 67, 68, 65, 66, 67, 32, 65, 66, 67, 68, 
	65, 66, 67, 68, 32, 66, 67, 68, 65, 32, 67, 32, 65, 66, 67, 68, 0}; /* our text buffer we want to edit */
	static uint select_start = 140; /* the start of our selection */
	static uint select_end = 350; /* the end of our selection */
	static uint line_start = 0; /* scroll start of our text*/


	/* temp buffers */

	uint end; /* how long we will draw the text */
	uint i;
	float curser_start; /* the position of our selection start */
	float curser_end; /* the position of our selection end */

	/* input */

	float pointer_x; /* the position of the pointer in relation to the text*/
	float pointer_y; /* the position of the pointer in relation to the text*/
	boolean mouse_button; /* is the mouse clicked */
	boolean last_mouse_button; /* was the the mouse clicked last frame */
	boolean event = FALSE; /* is a key beeing pressed*/
	static char letter = 0; /* letter we want to input */

/*	{
		static float time = 0;
		time += 0.003;
		line_start = (uint)(10.0 + 10.0 * sin(time));
	}*/

	/* load input */

	{
		BInputState *input;
		input = betray_get_input_state();
		pointer_x = input->pointer_x;
		pointer_y = input->pointer_y;
		mouse_button = input->mouse_button[0];
		last_mouse_button = input->last_mouse_button[0];
		if(input->event_count > 0 && input->event[0].state)
		{
			letter = input->event[0].button;
			event = TRUE;
		}
	}
	sui_text_box_edit_draw(size, spacing, line_spacing, length, text, lines, &line_start, select_start, select_end, TRUE);
	sui_text_box_edit_mouse(size, spacing, length, line_spacing, text, lines, &line_start, &select_start, &select_end, mouse_button, last_mouse_button, pointer_x, pointer_y);


	if(event == TRUE)
	{
		if(letter == 19) /* forward key */
		{
			sui_text_edit_forward(text, &select_start, &select_end);
		}
		else if(letter == 20) /* back key */
		{
			sui_text_edit_back(text, &select_start, &select_end);
		}
		else if(letter == 127) /* delete */
		{
			sui_text_edit_delete(text, 2000, &select_start, &select_end);
		}
		else if(letter == 23) /* end */
		{
			sui_text_edit_end(text, 2000, &select_start, &select_end);
		}
		else if(letter == 22) /* home */
		{
			sui_text_edit_home(text, 2000, &select_start, &select_end);
		}
		else if(letter == 8) /* backspace */
		{
			sui_text_edit_backspace(text, 2000, &select_start, &select_end);
		}
		else
		{
			sui_text_edit_insert_character(text, 2000, &select_start, &select_end, letter);
		}
	}
	{
		char temp_text[2000];
		glPushMatrix();
		glTranslatef(0.2, 0.2, 0.2);
		sprintf(temp_text, "line = %uselect start = %u end = %u curr = %u", line_start, select_start, select_end, 
			sui_text_block_hit_test(size, spacing, length, line_spacing, lines, text, pointer_x, pointer_y));
		sui_text_line_draw(size, spacing, temp_text, -1, 10);
		glPopMatrix();
	}
}