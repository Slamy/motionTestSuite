/*
 * VerticalText.cpp
 *
 *  Created on: 11.06.2020
 *      Author: andre
 */

#include "VerticalText.h"

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

const char* loremIpsum = "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, "
						 "sed diam nonumy eirmod tempor invidunt ut labore et dolore magna "
						 "aliquyam erat, sed diam voluptua. At vero eos et accusam et justo "
						 "duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata "
						 "sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, "
						 "consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut "
						 "labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et "
						 "accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no "
						 "sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit "
						 "amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut "
						 "labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et "
						 "accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea "
						 "takimata sanctus est Lorem ipsum dolor sit amet.";

void VerticalText::draw()
{
	glClearColor(1, 1, 1, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, screen_width, screen_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, screen_width, 0, screen_height);

	int y = 0;
	int x = 0;

	int spacing_y		= surfaceMessage->h + 50;
	int spacing_x		= surfaceMessage->w + 50;
	static int offset_y = 0;

	for (y = -spacing_y + offset_y; y < screen_height; y += spacing_y)
	{
		for (x = 30; x < screen_width; x += spacing_x)
		{

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glTranslatef(x, y, 0);
			glColor3f(1, 1, 1);

			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texture_text);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glBegin(GL_QUADS);
			glTexCoord2f(0, 0);
			glVertex2f(0, surfaceMessage->h);

			glTexCoord2f(0, 1);
			glVertex2f(0, 0);

			glTexCoord2f(1, 1);
			glVertex2f(surfaceMessage->w, 0);

			glTexCoord2f(1, 0);
			glVertex2f(surfaceMessage->w, surfaceMessage->h);
			glEnd();

			glDisable(GL_TEXTURE_2D);
		}
	}

	frame_cnt++;

	offset_y += pixels_per_frame;
	if (offset_y >= spacing_y)
		offset_y -= spacing_y;

	char msg[100];
	snprintf(msg, sizeof(msg), "%d FPS\n%d PPS\n%d PPF", frames_per_second, pixels_per_second, pixels_per_frame);
	msg[sizeof(msg) - 1] = '\0';
	drawText(10, screen_height - 10, msg);
}

VerticalText::VerticalText()
{
	SDL_Color White = {0, 0, 0};
	surfaceMessage	= TTF_RenderText_Blended_Wrapped(font, loremIpsum, White, 1000);

	glGenTextures(1, &texture_text);
	glBindTexture(GL_TEXTURE_2D, texture_text);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int stride = surfaceMessage->pitch / surfaceMessage->format->BytesPerPixel;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, stride, surfaceMessage->h, 0, GL_BGRA, GL_UNSIGNED_BYTE,
				 surfaceMessage->pixels);
}

VerticalText::~VerticalText()
{
	glDeleteTextures(1, &texture_text);
	SDL_FreeSurface(surfaceMessage);
}
