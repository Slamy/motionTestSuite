/*
 * StrobeCrossTalk.cpp
 *
 *  Created on: 11.06.2020
 *      Author: andre
 */

#include "StrobeCrossTalk.h"

#include <GL/gl.h>
#include <GL/glu.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_ttf.h>
#include <assert.h>
#include <boost/program_options.hpp>
#include <cstdio>
#include <functional>
#include <iostream>
#include <sys/time.h>
#include <unistd.h>

#include "motionTestSuite.h"

static void drawVerticalLine(int x)
{
	glLineWidth(1.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(x, 0);
	glVertex2f(x, screen_height);
	glEnd();
}

void StrobeCrossTalk::draw()
{
	if (out_of_sync_cnt)
		glClearColor(0.4f, 0.0f, 0.0f, 0);
	else
		glClearColor(0.0f, 0.5f, 0.5f, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, screen_width, screen_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, screen_width, 0, screen_height);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	int y = 0;
	int x = 0;

	int spacing_y		= 107;
	int spacing_x		= 160;
	static int offset_x = 0;

	int y_row = 0;

	static int verticalLineX = 0;

	for (y = spacing_y / 2; y < screen_height; y += spacing_y)
	{
		for (x = -spacing_x + offset_x; x < screen_width; x += spacing_x)
		{
			drawUfo(x, y);
		}

		y_row++;
	}

	//drawVerticalLine(verticalLineX);

	frame_cnt++;

	offset_x += pixels_per_frame;
	if (offset_x >= spacing_x)
		offset_x -= spacing_x;

	verticalLineX += pixels_per_frame;
	if (verticalLineX >= screen_width)
		verticalLineX -= screen_width;

	char msg[100];
	snprintf(msg, sizeof(msg), "%.2f FPS\n%d PPS\n%d PPF", frames_per_second, pixels_per_second, pixels_per_frame);
	msg[sizeof(msg) - 1] = '\0';
	drawText(10, screen_height - 10, msg);
}
