/**
 * @file strobeCrossTalk.cpp
 * @author Slamy
 *
 * Based on the example from https://geometrian.com/programming/tutorials/texturegl/index.php
 * Currently only tested with SDL2 on Debian Testing.
 */

#include <GL/gl.h>
#include <GL/glu.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <assert.h>
#include <boost/program_options.hpp>
#include <cstdio>
#include <iostream>
#include <sys/time.h>
#include <unistd.h>

static SDL_Window* window;

int screen_width  = 2560;
int screen_height = 1440;

GLuint texture;

int textureW		  = 0;
int textureH		  = 0;
int pixels_per_frame  = 6;
int pixels_per_second = 960;
static int frame_cnt  = 0;

SDL_Surface* surf = nullptr;

namespace po = boost::program_options;

static void init_texture(void)
{
	assert(surf);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surf->w, surf->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, surf->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	textureW = surf->w;
	textureH = surf->h;
	printf("Texture %d %d\n", textureW, textureH);

	// Unload SDL's copy of the data; we don't need it anymore because OpenGL now stores it in the texture.
	SDL_FreeSurface(surf);
}
static void deinit_texture(void)
{
	// Deallocate texture.  A lot of people forget to do this.
	glDeleteTextures(1, &texture);
}

static bool get_input(void)
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			return false; // The little X in the window got pressed
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				return false;
				break;
			}

			break;
		}
	}
	return true;
}

static void drawUfo(int x, int y)
{
	// move the rendered surface to a different place
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(x, y, 0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Set our loaded texture as the current 2D texture (this isn't actually technically necessary since our
	// texture was never unselected from above, but this is most clear)
	glBindTexture(GL_TEXTURE_2D, texture);
	// Tell OpenGL that all subsequent drawing operations should try to use the current 2D texture
	glEnable(GL_TEXTURE_2D);

	// perform the actual drawing of the UFO as a quad
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2f(0, textureH);

	glTexCoord2f(0, 1);
	glVertex2f(0, 0);

	glTexCoord2f(1, 1);
	glVertex2f(textureW, 0);

	glTexCoord2f(1, 0);
	glVertex2f(textureW, textureH);
	glEnd();

	// Tell OpenGL that all subsequent drawing operations should NOT try to use the current 2D texture
	glDisable(GL_TEXTURE_2D);
}

static void drawVerticalLine(int x)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(x, 0);
	glVertex2f(x, screen_height);
	glEnd();
}

static void drawStrobeCrossTalk(void)
{
	glClearColor(0.3f, 0.3f, 0.3f, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, screen_width, screen_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, screen_width, 0, screen_height);

	int y = 0;
	int x = 0;

	int spacing_y		= 107;
	int spacing_x		= 140;
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

	drawVerticalLine(verticalLineX);

	frame_cnt++;

	offset_x += pixels_per_frame;
	if (offset_x >= spacing_x)
		offset_x -= spacing_x;

	verticalLineX += pixels_per_frame;
	if (verticalLineX >= screen_width)
		verticalLineX -= screen_width;

	SDL_GL_SwapWindow(window);
}

static void drawUfoBackground(int y, uint8_t r, uint8_t g, uint8_t b)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, y, 0);

	glColor3ub(r, g, b);

	glBegin(GL_QUADS);
	glVertex2f(0, textureH + 5);
	glVertex2f(0, -5);
	glVertex2f(screen_width, -5);
	glVertex2f(screen_width, textureH + 5);
	glEnd();
}

static void drawPursuitPattern(int centerY, int pursuit_spacing_x, int offset_x)
{
	int stickLen = 4;

	glBegin(GL_LINES);
	glColor3f(0.5f, 0.0f, 0.5f);

	glVertex2f(0, centerY + textureH + 10);
	glVertex2f(screen_width, centerY + textureH + 10);

	glVertex2f(0, centerY + textureH + 10 + stickLen * 4 + 1);
	glVertex2f(screen_width, centerY + textureH + 10 + stickLen * 4 + 1);
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);
	int x;

	for (x = -pursuit_spacing_x + offset_x; x < screen_width; x += pursuit_spacing_x)
	{
		glBegin(GL_LINES);
		glColor3f(1.0f, 1.0f, 1.0f);

		int y = centerY + textureH + 10 + (frame_cnt % 4) * stickLen;

		glVertex2f(x, y);
		glVertex2f(x, y + stickLen);
		glEnd();
	}
}

static void drawPursuitCamera(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, screen_width, screen_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, screen_width, 0, screen_height);

	int y = 0;
	int x = 0;

	int spacing_y		  = 130;
	int ufo_spacing_x	  = 140;
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
	for (y = 0; y < 3; y++)
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

	frame_cnt++;

	offset_x += pixels_per_frame;
	if (offset_x >= ufo_spacing_x)
		offset_x -= ufo_spacing_x;

	SDL_GL_SwapWindow(window);
}

double getTime()
{
	struct timeval t;
	gettimeofday(&t, NULL);
	return t.tv_sec + ((double)t.tv_usec) / (1000.0 * 1000.0);
}

int main(int argc, char* argv[])
{
	int opt;
	po::options_description desc("Allowed options");
	bool activateVSync		 = false;
	bool activateFullScreen	 = false;
	bool strobeCrossTalkTest = false;

	int msdelay = 0;

	// clang-format off
	desc.add_options()
	    ("help", "produce help message")
	    ("vsync", po::bool_switch(&activateVSync)->default_value(false), "Activate VSync")
	    ("msdelay", po::value<int>(&msdelay)->default_value(0), "Force delay between each frame")
		("strobe", po::bool_switch(&strobeCrossTalkTest)->default_value(false), "Activates strobe cross talk test")
		("full", po::bool_switch(&activateFullScreen)->default_value(false), "Full screen mode")
		;
	// clang-format on
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help"))
	{
		std::cout << desc << "\n";
		return 1;
	}

	int sdlWindowFlags = SDL_WINDOW_OPENGL;
	if (activateFullScreen)
	{
		sdlWindowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	}

	SDL_Init(SDL_INIT_EVERYTHING | SDL_INIT_NOPARACHUTE);

	// Load the image from the file into SDL's surface representation
	surf = SDL_LoadBMP("testpattern.bmp");
	if (surf == NULL)
	{
		printf("Error: \"%s\"\n", SDL_GetError());
		return 1;
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	window = SDL_CreateWindow("Slamy's SDL Motion Test Suite", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
							  screen_width, screen_height, sdlWindowFlags);
	SDL_GLContext context = SDL_GL_CreateContext(window);

	glDisable(GL_DEPTH_TEST);
	SDL_GL_SetSwapInterval(activateVSync ? 1 : 0);
	SDL_GetWindowSize(window, &screen_width, &screen_height);

	init_texture();

	time_t lastT		  = time(NULL);
	int frames_per_second = 0;

	while (true)
	{
		if (!get_input())
			break;

		if (strobeCrossTalkTest)
			drawStrobeCrossTalk();
		else
			drawPursuitCamera();

		if (msdelay)
			usleep(msdelay * 1000);

		time_t nowT = time(NULL);
		frames_per_second++;
		if (nowT != lastT)
		{
			lastT			 = nowT;
			pixels_per_frame = roundf((float)pixels_per_second / (float)frames_per_second);
			if (pixels_per_frame > 20)
				pixels_per_frame = 20;
			// printf("%d %d -> %d\n", frames_per_second, pixels_per_frame, pixels_per_second);
			frames_per_second = 0;
		}
	}

	deinit_texture();

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}
