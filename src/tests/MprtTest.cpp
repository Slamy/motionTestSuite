/*
 * MprtTest.cpp
 *
 *  Created on: 15.06.2020
 *      Author: andre
 */

#include <tests/MprtTest.h>

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

void MprtTest::draw()
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

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, screen_height / 2 - screen_height / 4, 0);

	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);

	int x;
	int y;
	bool evenChecker	= true;
	static int offset_x = 0;
	for (y = 0; y < screen_height / 2; y += checkerBoardSize)
	{
		for (x = -checkerBoardSize * 10; x < screen_width + checkerBoardSize; x += checkerBoardSize * 2)
		{
			int drawX = x + offset_x;
			if (evenChecker)
				drawX += checkerBoardSize;

			glVertex2f(drawX, y);
			glVertex2f(drawX, y + checkerBoardSize);
			glVertex2f(drawX + thickness, y + checkerBoardSize);
			glVertex2f(drawX + thickness, y);
		}

		evenChecker = !evenChecker;
	}
	glEnd();

	offset_x += pixelsPerFrame;
	if (offset_x >= checkerBoardSize * 10)
		offset_x -= checkerBoardSize * 10;

	int pixelsPerSecond = pixelsPerFrame * frames_per_second;

	static int ufo_x = -pixelsPerFrame * 20;
	ufo_x += pixelsPerFrame;
	if (ufo_x > screen_width)
		ufo_x = -pixelsPerFrame * 20;
	drawUfo(ufo_x, screen_height / 2);

	// This is directly taken from the JavaScript source code of:
	// https://www.testufo.com/mprt#background=000000&foreground=ffffff&size=12&thickness=2&pursuit=0&ppf=0

	double mcr = static_cast<double>(pixelsPerSecond) /
				 (static_cast<double>(checkerBoardSize) - static_cast<double>(thickness) / 2.0);
	double mprt = 1000.0 / mcr;

	double ms_per_frame = 1000.0 / static_cast<double>(frames_per_second);

	char msg[300];
	snprintf(msg, sizeof(msg),
			 "Checkerboard Size: %d Pixels (change with E and R)\n"
			 "Thickness: %d Pixels (change with D and F)\n"
			 "Pixels per Frame %d (change with C and V)\n"
			 "Frame Rate %.2f fps / %.2f mspf\n"
			 "Pixels per Second %d\n"
			 "Motion Clarity Ratio (MCR): %.0f\n"
			 "Persistence: %.1fms (MPRT)\n",
			 checkerBoardSize, thickness, pixelsPerFrame, frames_per_second, ms_per_frame, pixelsPerSecond, mcr, mprt);
	msg[sizeof(msg) - 1] = '\0';
	drawText(10, screen_height - 10, msg);
}
