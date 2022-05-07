/*
 * PursuitCamera.cpp
 *
 *  Created on: 11.06.2020
 *      Author: andre
 */

#include "PursuitCamera.h"

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

static void drawPursuitPattern(int centerY, int pursuit_spacing_x, int offset_x)
{
	int stickLen = 4;

	glLineWidth(1.0f);

	glBegin(GL_LINES);
	glColor3f(0.5f, 0.0f, 0.5f);

	glVertex2f(0, centerY + moving_target_height + 10);
	glVertex2f(screen_width, centerY + moving_target_height + 10);

	glVertex2f(0, centerY + moving_target_height + 10 + stickLen * 4 + 1);
	glVertex2f(screen_width, centerY + moving_target_height + 10 + stickLen * 4 + 1);
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);
	int x;

	for (x = -pursuit_spacing_x + offset_x; x < screen_width; x += pursuit_spacing_x)
	{
		glBegin(GL_LINES);
		glColor3f(1.0f, 1.0f, 1.0f);

		int y = centerY + moving_target_height + 10 + (frame_cnt % 4) * stickLen;

		glVertex2f(x, y);
		glVertex2f(x, y + stickLen);
		glEnd();
	}
}

static void drawUfoBackground(int y, uint8_t r, uint8_t g, uint8_t b)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, y, 0);

	glColor3ub(r, g, b);

	glBegin(GL_QUADS);
	glVertex2f(0, moving_target_height + 5);
	glVertex2f(0, -5);
	glVertex2f(screen_width, -5);
	glVertex2f(screen_width, moving_target_height + 5);
	glEnd();
}

void PursuitCamera::draw()
{
	if (out_of_sync_cnt)
		glClearColor(0.4f, 0.0f, 0.0f, 0);
	else
		glClearColor(0.0f, 0.0f, 0.0f, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, screen_width, screen_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, screen_width, 0, screen_height);

	int y = 0;
	int x = 0;

	int spacing_y		  = moving_target_height + 36;
	int ufo_spacing_x	  = 160;
	int pursuit_spacing_x = ufo_spacing_x / 2;
	static int offset_x	  = 0;

	int centerY = screen_height / 2;
	int startY	= centerY - spacing_y;

	// UFO Background Area
	drawUfoBackground(startY, 0, 64, 64);
	drawUfoBackground(startY + spacing_y, 0, 128, 128);
	drawUfoBackground(startY + spacing_y * 2, 0, 192, 192);

	// UFOs
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor3f(1.0f, 1.0f, 1.0f);
	for (y = -1; y < 3; y++)
	{
		for (x = -ufo_spacing_x + offset_x; x < screen_width; x += ufo_spacing_x)
		{
			drawUfo(x, startY + spacing_y * y);
		}
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor3f(1.0f, 1.0f, 1.0f);

	// Pursuit Pattern
	drawPursuitPattern(startY, pursuit_spacing_x, offset_x);
	drawPursuitPattern(startY + spacing_y, pursuit_spacing_x, offset_x);
	drawPursuitPattern(startY + spacing_y * 2, pursuit_spacing_x, offset_x);
	drawPursuitPattern(startY - spacing_y, pursuit_spacing_x, offset_x);
	drawPursuitPattern(startY - spacing_y * 2, pursuit_spacing_x, offset_x);

	frame_cnt++;

	offset_x += pixels_per_frame;
	if (offset_x >= ufo_spacing_x)
		offset_x -= ufo_spacing_x;

	char msg[100];
	snprintf(msg, sizeof(msg), "%.2f frames per second\n%d scrolled pixels per second\n%d scrolled pixels per frame",
			 frames_per_second, pixels_per_second, pixels_per_frame);
	msg[sizeof(msg) - 1] = '\0';
	drawText(10, screen_height - 10, msg);
}
