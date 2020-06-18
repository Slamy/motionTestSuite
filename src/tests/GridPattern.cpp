/*
 * GridPattern.cpp
 *
 *  Created on: 14.06.2020
 *      Author: andre
 */

#include <tests/GridPattern.h>

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

GridPattern::GridPattern(bool focus)
{
	this->focus	 = focus;
	gridCellSize = 32;
	while (((screen_width % gridCellSize) == 0) || ((screen_height % gridCellSize) == 0))
	{
		// If this is the case there is part of the grid missing. Select another grid size.
		gridCellSize--;
	}

	std::cout << "grid cell size " << gridCellSize << std ::endl;
	std::cout << "grid pattern border " << screen_width % gridCellSize << " " << screen_height % gridCellSize
			  << std ::endl;

	cellsHorizontal = screen_width / gridCellSize;
	cellsVertical	= screen_height / gridCellSize;
	offsetX			= (screen_width % gridCellSize) / 2;
	offsetY			= (screen_height % gridCellSize) / 2;

	std::cout << "grid offsetX " << offsetX << std ::endl;
	std::cout << "grid offsetY " << offsetY << std ::endl;
}

void GridPattern::draw()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, screen_width, screen_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, screen_width, 0, screen_height);

	if (focus)
		focusGrid();
	else
		convergenceGrid();
}

void GridPattern::convergenceGrid()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor3f(1.0f, 1.0f, 1.0f);
	glLineWidth(1.0f);

	glBegin(GL_LINES);

	// draw horizontal lines
	int y, x;
	for (y = 0; y < cellsVertical + 1; y++)
	{
		glVertex2i(offsetX, offsetY + y * gridCellSize);
		glVertex2i(screen_width - offsetX, offsetY + y * gridCellSize);
	}

	// draw vertical lines
	for (x = 0; x < cellsHorizontal + 1; x++)
	{
		glVertex2i(1 + offsetX + x * gridCellSize, offsetY - 1);
		glVertex2i(1 + offsetX + x * gridCellSize, screen_height - offsetY);
	}
	glEnd();

	glPointSize(3.0f); // set point size to 10 pixels
	glBegin(GL_POINTS);

	// draw points in the center
	for (y = 0; y < cellsVertical; y++)
	{
		for (x = 0; x < cellsHorizontal; x++)
		{
			glVertex2i(offsetX + x * gridCellSize + gridCellSize / 2, offsetY + y * gridCellSize + gridCellSize / 2);
		}
	}

	glEnd();
}

void GridPattern::focusGrid()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor3f(1.0f, 1.0f, 1.0f);

	int y, x;
	int i;
	glMatrixMode(GL_MODELVIEW);
	for (y = 0; y < screen_height; y += 64 * 2)
	{
		for (x = 0; x < screen_width; x += 64 * 2)
		{
			glLoadIdentity();
			glTranslatef(x, y, 0);
			glLineWidth(1.0f);

			glBegin(GL_LINES);
			for (i = 0; i < 64; i += 2)
			{
				glVertex2i(i, 0);
				glVertex2i(i, 64);
			}
			glEnd();

			glTranslatef(64, 1, 0);

			glBegin(GL_LINES);
			for (i = 0; i < 64; i += 2)
			{
				glVertex2i(-1, i);
				glVertex2i(64, i);
			}
			glEnd();

			// bigger lines
			glLineWidth(2.0f);
			glLoadIdentity();
			glTranslatef(x, y + 64, 0);
			glBegin(GL_LINES);
			for (i = 0; i < 64; i += 4)
			{
				glVertex2i(i, 0);
				glVertex2i(i, 64);
			}
			glEnd();

			glTranslatef(64, 1, 0);

			glBegin(GL_LINES);
			for (i = 0; i < 64; i += 4)
			{
				glVertex2i(-1, i);
				glVertex2i(64, i);
			}
			glEnd();
		}
	}
}
