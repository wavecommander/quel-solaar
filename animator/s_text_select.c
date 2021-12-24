#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "seduce.h"
#include "s_text.h"
/*
#define GL_ZERO                           0
#define GL_ONE                            1
#define GL_SRC_COLOR                      0x0300
#define GL_ONE_MINUS_SRC_COLOR            0x0301
#define GL_SRC_ALPHA                      0x0302
#define GL_ONE_MINUS_SRC_ALPHA            0x0303
#define GL_DST_ALPHA                      0x0304
#define GL_ONE_MINUS_DST_ALPHA            0x0305

#define GL_DST_COLOR                      0x0306
#define GL_ONE_MINUS_DST_COLOR            0x0307
#define GL_SRC_ALPHA_SATURATE             0x0308

*/
void sui_text_line_cursur_draw(float pos_x, float pos_y, float size, float curser_x, float curser_y)
{
	float array[4];
	array[0] = pos_x + curser_x;
	array[1] = pos_y + curser_y + size * 2;
	array[2] = pos_x + curser_x;
	array[3] = pos_y + curser_y - size;
	glVertexPointer(2, GL_FLOAT , 0, array);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
	glDrawArrays(GL_LINES, 0, 2);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void sui_text_line_selection_draw(float pos_x, float pos_y, float size, float curser_start, float curser_end, float curser_y)
{
	float array[16];
	array[0] = pos_x + curser_start;
	array[1] = pos_y + curser_y + size * 2;
	array[2] = pos_x + curser_start;
	array[3] = pos_y + curser_y - size;
	array[4] = pos_x + curser_end;
	array[5] = pos_y + curser_y - size;
	array[6] = pos_x + curser_end;
	array[7] = pos_y + curser_y + size * 2;
	glVertexPointer(2, GL_FLOAT , 0, array);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
	glDrawArrays(GL_QUADS, 0, 4);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}