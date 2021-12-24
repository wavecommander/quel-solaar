#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "seduce.h"
#include "s_text.h"

boolean sui_util_text_insert_character(char *text, uint size, uint curser_start, uint curser_end, char *insert)
{
	uint i, in_end, out_end;
	char a[1280], b[1280];
	if(curser_start > curser_end)
	{
		i = curser_start;
		curser_start = curser_end;
		curser_end = i;
	}
	for(out_end = 0; text[out_end] != 0; out_end++);
	for(in_end = 0; insert[in_end] != 0; in_end++);
	if(out_end + in_end + curser_start - curser_end >= size)
		return FALSE;
	if(in_end > curser_end - curser_start)
	{	
		for(i = out_end; i >= curser_end && i != -1; i--)
			text[i + in_end - (curser_end - curser_start)] = text[i];
		for(i = 0; i < in_end; i++)
			text[curser_start + i] = insert[i];
	}else
	{	
		for(i = curser_end; i < out_end; i++)
			text[i + in_end - (curser_end - curser_start)] = text[i];
		text[i + in_end - (curser_end - curser_start)] = 0;
		for(i = 0; i < in_end; i++)
			text[curser_start + i] = insert[i];
	}
	return TRUE;
}


void sui_text_line_edit_draw(float pos_x, float pos_y, float size, float spacing, float length, char *text, uint *scroll_start, uint select_start, uint select_end)
{
	float curser_start, curser_end;
	uint end;
	end = sui_text_line_hit_test(size, spacing, &text[*scroll_start], length) + *scroll_start;
	/* find and draw the selection */

	if(select_start <= *scroll_start) // does the select start before the beginning of the visable text
		curser_start = 0; // set the start to zero
	else
		curser_start = sui_text_line_length(size, spacing, &text[*scroll_start], select_start - *scroll_start); // find the position of the begining of the select

	if(select_start == select_end) /* are we in cursor or select mode? */
		sui_text_line_cursur_draw(pos_x, pos_y, size, curser_start, 0); /* draw the cursor*/
	else 
	{
		if(select_end > end) // does the select start before the beginning of the visable text
			curser_end = length; // set the end to the length of the box
		else
			curser_end = sui_text_line_length(size, spacing, &text[*scroll_start], select_end - *scroll_start); // find the position of the end of the select
		
		sui_text_line_selection_draw(pos_x, pos_y, size, curser_start, curser_end, 0); /* draw the selectoion*/
	}
	if(select_end < *scroll_start && *scroll_start != 0) /* if we are at the begining of our view and there is more text scroll back */
		(*scroll_start)--;
	if(select_end > end && text[end] != 0)  /* if we are att the end of our view and there is more text scroll forward */
		(*scroll_start)++;
}


void sui_text_box_edit_draw2(float size, float spacing, float line_spacing, float length, char *text, uint lines, uint *line_start, uint select_start, uint select_end, boolean draw_curser)
{
	float curser_start, curser_end;
	uint end, scroll_start = 0, i;

	for(i = 0; i < *line_start; i++)
		scroll_start += sui_text_block_line_end(&text[scroll_start], spacing, size, length);


	sui_text_block_draw(size, spacing, length, line_spacing, lines, &text[scroll_start]);

//	end = sui_text_line_hit_test(size, spacing, &text[scroll_start], length) + *scroll_start;
//	sui_text_line_draw(size, spacing, &text[scroll_start], end - *scroll_start, uint samples); // draw the text

	/* find and draw the selection */
	if(!draw_curser)
		return;
	if(select_start > select_end)
	{
		i = select_end;
		select_end = select_start;
		select_start = i;
	}
	{
		float x, y;
		sui_text_block_pos(size, spacing, length, line_spacing, &text[scroll_start], select_start - scroll_start, &x, &y);
		sui_draw_2d_line_gl(x - 0.1, y - 0.1, x + 0.1, y + 0.1, 1, 1, 1, 1);
		sui_draw_2d_line_gl(x - 0.1, y + 0.1, x + 0.1, y - 0.1, 1, 1, 1, 1);
	}
	glPushMatrix();
	for(i = 0; text[scroll_start] != 0; i++)
	{


		end = sui_text_block_line_end(&text[scroll_start], spacing, size, length) + scroll_start;
		
		if(scroll_start < select_end && end > select_start)
		{
			if(select_start <= scroll_start) // does the select start before the beginning of the visable text
				curser_start = 0; // set the start to zero
			else
//				curser_start = sui_text_line_length(size, spacing, &text[scroll_start], select_start - scroll_start); // find the position of the begining of the select
				sui_text_block_pos(size, spacing, length, line_spacing, &text[scroll_start], select_start - scroll_start, &curser_start, NULL);
		
			if(select_start == select_end) /* are we in cursor or select mode? */
				sui_text_line_cursur_draw(0, 0, size, curser_start, 0); /* draw the cursor*/
			else 
			{
				if(select_end > end) // does the select start before the beginning of the visable text
					curser_end = length; // set the end to the length of the box
				else
//					curser_end = sui_text_line_length(size, spacing, &text[scroll_start], select_end - scroll_start); // find the position of the end of the select
					sui_text_block_pos(size, spacing, length, line_spacing, &text[scroll_start], select_start - scroll_start, &curser_end, NULL);				
				sui_text_line_selection_draw(0, 0, size, curser_start, curser_end, 0); /* draw the selectoion*/
			}
		}
		glTranslatef(0, -size * line_spacing, 0);
		scroll_start = end;	
	}
	glPopMatrix();



//	if(select_end < *scroll_start && *scroll_start != 0) /* if we are at the begining of our view and there is more text scroll back */
//		(*scroll_start)--;
//	if(select_end > end && text[end] != 0)  /* if we are att the end of our view and there is more text scroll forward */
//		(*scroll_start)++;
}

void sui_text_box_edit_draw(float size, float spacing, float line_spacing, float length, char *text, uint lines, uint *line_start, uint select_start, uint select_end, boolean draw_curser)
{
	float curser_start, curser_end;
	float x_start, y_start, x_end, y_end;
	uint end, scroll_start = 0, i;

	for(i = 0; i < *line_start; i++)
		scroll_start += sui_text_block_line_end(&text[scroll_start], spacing, size, length);
	sui_text_block_draw(size, spacing, length, line_spacing, lines, &text[scroll_start]);

	if(!draw_curser)
		return;
	if(select_start > select_end)
	{
		i = select_end;
		select_end = select_start;
		select_start = i;
	}
	if(select_end < scroll_start)
		return;
	if(select_start < scroll_start)
		select_start = scroll_start;

	if(select_start < scroll_start)
		x_start = y_start = 0;
	else
		sui_text_block_pos(size, spacing, length, line_spacing, &text[scroll_start], select_start - scroll_start, &x_start, &y_start);
	sui_draw_2d_line_gl(x_start - 0.1, y_start - 0.1, x_start + 0.1, y_start + 0.1, 1, 1, 1, 1);
	sui_draw_2d_line_gl(x_start - 0.1, y_start + 0.1, x_start + 0.1, y_start - 0.1, 1, 1, 1, 1);

	if(select_start == select_end)
	{
		sui_text_line_cursur_draw(0, 0, size, x_start, y_start);	
		return;
	}

	sui_text_block_pos(size, spacing, length, line_spacing, &text[scroll_start], select_end - scroll_start, &x_end, &y_end);
	sui_draw_2d_line_gl(x_end - 0.1, y_end - 0.1, x_end + 0.1, y_end + 0.1, 1, 1, 1, 1);
	sui_draw_2d_line_gl(x_end - 0.1, y_end + 0.1, x_end + 0.1, y_end - 0.1, 1, 1, 1, 1);
	
//	for(i = (uint)((y_start + 0.5 * size) / (size * line_spacing)); i < lines; i++)
	for(i = 0; i < 100; i++)
	{
		if(y_start + 0.5 * size > y_end && y_start - 0.5 * size < y_end)
		{
			sui_text_line_selection_draw(0, 0, size, x_start, x_end, y_start);
			return;
		}else
		{
			sui_text_line_selection_draw(0, 0, size, x_start, length, y_start);
		}
		y_start -= size * line_spacing;
		x_start = 0;
	}
/*
extern void		sui_text_line_cursur_draw(float size, float curser_x, float curser_y);
extern void		sui_text_line_selection_draw(float size, float curser_start, float curser_end, float curser_y);
*/
	glPushMatrix();

	glPopMatrix();
}

void sui_text_line_edit_mouse(float size, float spacing, float length, char *text, uint *scroll_start, uint *select_start, uint *select_end, boolean mouse_button, boolean mouse_button_last, float pointer_x)
{
	if(mouse_button) /* is the mouse in action */
	{
		uint i;
		uint end;
		end = sui_text_line_hit_test(size, spacing, &text[*scroll_start], length) + *scroll_start;
		i = sui_text_line_hit_test(size, spacing, &text[*scroll_start], pointer_x); /* find the place of the select*/
		if(!mouse_button_last) // just clicked
			*select_start = *select_end = i;
		else // keep clicking
			*select_end = i;

		if(*select_end < *scroll_start && scroll_start != 0) /* if we are at the begining of our view and there is more text scroll back */
			(*scroll_start)--;
		if(*select_end > end && text[end] != 0)  /* if we are att the end of our view and there is more text scroll forward */
			(*scroll_start)++;
	}	
}
/*
	sui_text_box_edit_mouse(size, spacing, length, line_spacing, text, lines, &line_start, &select_start, &select_end, mouse_button, last_mouse_button, pointer_x, pointer_y);
	sui_text_block_hit_test(size, spacing, length, line_spacing, lines, text, pointer_x, pointer_y)
*/

void sui_text_box_edit_mouse(float size, float spacing, float line_spacing, float length, char *text, uint lines, uint *line_start, uint *select_start, uint *select_end, boolean mouse_button, boolean mouse_button_last, float pointer_x, float pointer_y)
{
	if(mouse_button) /* is the mouse in action */
	{
		uint i, scroll_start = 0;
		uint end;

		for(i = 0; i < *line_start; i++)
			scroll_start += sui_text_block_line_end(&text[scroll_start], spacing, size, length);

		i = sui_text_block_hit_test(size, spacing, line_spacing, length, lines, &text[scroll_start], pointer_x, pointer_y);// + scroll_start;

		if(!mouse_button_last) // just clicked
			*select_start = *select_end = i;
		else // keep clicking
			*select_end = i;

		if(i == 0 && *line_start != 0) /* if we are at the begining of our view and there is more text scroll back */
			(*line_start)--;
//		if(*select_end > end && text[end] != 0)  /* if we are att the end of our view and there is more text scroll forward */
//			(*line_start)++;
	}
}

void sui_text_edit_forward(char *text, uint *select_start, uint *select_end)
{
	if(select_end > select_start)
		*select_start = *select_end;
	else
		*select_end = *select_start;
	if(text[*select_end] != 0)
		*select_end = *select_start = *select_start + 1;
}

void sui_text_edit_back(char *text, uint *select_start, uint *select_end)
{
	if(*select_end < *select_start)
		*select_start = *select_end;
	else
		*select_end = *select_start;
	if(*select_end != 0)
		*select_end = *select_start = *select_start - 1;
}

void sui_text_edit_delete(char *text, uint length, uint *select_start, uint *select_end)
{
	char insert[1] = {0};
	if(*select_end == *select_start && text[*select_end] != 0)
		(*select_end)++;
	sui_util_text_insert_character(text, length, *select_start, *select_end, insert);
	if(*select_start > *select_end)
		*select_start = *select_end;
	else
		*select_end = *select_start;
}

void sui_text_edit_end(char *text, uint length, uint *select_start, uint *select_end)
{
	uint i;
	for(i = 0; text[i] != 0; i++);
		*select_end = *select_start = i;
}

void sui_text_edit_home(char *text, uint length, uint *select_start, uint *select_end)
{
	*select_end = *select_start = 0;
}

void sui_text_edit_backspace(char *text, uint length, uint *select_start, uint *select_end)
{
	char insert[1] = {0};
	if(*select_end == *select_start && *select_end != 0)
		(*select_end)--;
	sui_util_text_insert_character(text, 2000, *select_start, *select_end, insert);
	if(*select_start > *select_end)
		*select_start = *select_end;
	else
		*select_end = *select_start;
}

void sui_text_edit_insert_character(char *text, uint length, uint *select_start, uint *select_end, char character)
{
	char insert[2] = {0, 0};
	insert[0] = character;
	sui_util_text_insert_character(text, length, *select_start, *select_end, insert);
	if(*select_start > *select_end)
		*select_start = *select_end = *select_end + 1;
	else
		*select_end = *select_start = *select_start + 1;
}

