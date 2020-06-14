/*
 * Contrast.cpp
 *
 *  Created on: 14.06.2020
 *      Author: andre
 */

#include "Contrast.h"

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

static void drawRect(int y, int rect_height, uint8_t r, uint8_t g, uint8_t b)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, y, 0);

	glColor3ub(r, g, b);

	glBegin(GL_QUADS);

	glColor3ub(0, 0, 0);
	glVertex2f(0, rect_height + 5);

	glColor3ub(0, 0, 0);
	glVertex2f(0, 0);

	glColor3ub(r, g, b);
	glVertex2f(screen_width, 0);

	glColor3ub(r, g, b);
	glVertex2f(screen_width, rect_height + 5);

	glEnd();
}

void Contrast::draw()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, screen_width, screen_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, screen_width, 0, screen_height);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	int rect_height = screen_height / 6;
	int startY		= screen_height / 2 - rect_height * 4 / 2;

	drawRect(startY + rect_height * 0, rect_height, 255, 0, 0);
	drawRect(startY + rect_height * 1, rect_height, 0, 255, 0);
	drawRect(startY + rect_height * 2, rect_height, 0, 0, 255);
	drawRect(startY + rect_height * 3, rect_height, 255, 255, 255);

	// draw a border.
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor3f(1, 1, 1);
	glBegin(GL_LINE_STRIP);

	glVertex2i(1, 0);
	glVertex2i(1, screen_height);
	glVertex2i(screen_width, screen_height);
	glVertex2i(screen_width, 1);
	glVertex2i(1, 1);

	glEnd();
}
