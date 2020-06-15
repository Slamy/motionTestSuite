/**
 * @file strobeCrossTalk.cpp
 * @author Slamy
 *
 * Based on the example from
 * https://geometrian.com/programming/tutorials/texturegl/index.php
 * https://moddb.fandom.com/wiki/SDL_ttf:Tutorials:Basic_Font_Rendering
 * https://moddb.fandom.com/wiki/SDL_ttf:Tutorials
 * Currently only tested with SDL2 on Debian Testing.
 *
 */

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
#include "tests/Contrast.h"
#include "tests/GridPattern.h"
#include "tests/MprtTest.h"
#include "tests/PursuitCamera.h"
#include "tests/SingleColor.h"
#include "tests/StrobeCrossTalk.h"
#include "tests/VerticalText.h"

static SDL_Window* window = nullptr;

int screen_width  = 1920;
int screen_height = 700;

GLuint texture_ufo;

int textureW		  = 0;
int textureH		  = 0;
int pixels_per_frame  = 6;
int pixels_per_second = 960;
int frame_cnt		  = 0;

int frames_per_second = 0;

SDL_Surface* surface_ufo = nullptr;

TTF_Font* font = nullptr;

std::shared_ptr<MotionTest> activeTest;

namespace po = boost::program_options;

static bool get_input(void);
static int msdelay = 0;

static void init_texture(void)
{
	assert(surface_ufo);

	glGenTextures(1, &texture_ufo);
	glBindTexture(GL_TEXTURE_2D, texture_ufo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface_ufo->w, surface_ufo->h, 0, GL_BGRA, GL_UNSIGNED_BYTE,
				 surface_ufo->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	textureW = surface_ufo->w;
	textureH = surface_ufo->h;
	printf("Texture %d %d\n", textureW, textureH);

	// Unload SDL's copy of the data; we don't need it anymore because OpenGL now stores it in the texture.
	SDL_FreeSurface(surface_ufo);
}
static void deinit_texture(void)
{
	// Deallocate texture.  A lot of people forget to do this.
	glDeleteTextures(1, &texture_ufo);
}

void drawText(int x, int y, const char* str)
{
	SDL_Color White				= {255, 255, 255};
	SDL_Surface* surfaceMessage = TTF_RenderText_Blended_Wrapped(font, str, White, 1000);
	GLuint texture_text;

	glGenTextures(1, &texture_text);
	glBindTexture(GL_TEXTURE_2D, texture_text);
	glEnable(GL_TEXTURE_2D);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surfaceMessage->w, surfaceMessage->h, 0, GL_BGRA, GL_UNSIGNED_BYTE,
				 surfaceMessage->pixels);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(x, y - surfaceMessage->h, 0);
	glColor3f(1, 1, 1);

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

	// Cleanup
	glDeleteTextures(1, &texture_text);
	SDL_FreeSurface(surfaceMessage);
}

void drawUfo(int x, int y)
{
	// move the rendered surface to a different place
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(x, y, 0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Set our loaded texture as the current 2D texture (this isn't actually technically necessary since our
	// texture was never unselected from above, but this is most clear)
	glBindTexture(GL_TEXTURE_2D, texture_ufo);
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
	int display				 = 0;

	// clang-format off
	desc.add_options()
	    ("help", "produce help message")
	    ("vsync", po::bool_switch(&activateVSync)->default_value(false), "Activate VSync")
	    ("msdelay", po::value<int>(&msdelay)->default_value(0), "Force delay between each frame")
		("display", po::value<int>(&display)->default_value(0), "Select display to use")
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

	if (TTF_Init() == -1)
	{
		printf("Unable to initialize SDL_ttf: %s \n", TTF_GetError());
		return 1;
	}

	const char* fontpath = "font.ttf";
	font				 = TTF_OpenFont(fontpath, 20);
	if (font == NULL)
	{
		printf("Unable to load font %s: %s \n", fontpath, TTF_GetError());
		return 1;
	}

	// Load the image from the file into SDL's surface representation
	surface_ufo = SDL_LoadBMP("testpattern.bmp");
	if (surface_ufo == NULL)
	{
		printf("Error: \"%s\"\n", SDL_GetError());
		return 1;
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	window = SDL_CreateWindow("Slamy's SDL Motion Test Suite", SDL_WINDOWPOS_CENTERED_DISPLAY(display),
							  SDL_WINDOWPOS_CENTERED_DISPLAY(display), screen_width, screen_height, sdlWindowFlags);

	SDL_GLContext context = SDL_GL_CreateContext(window);

	glDisable(GL_DEPTH_TEST);
	SDL_GL_SetSwapInterval(activateVSync ? 1 : 0);
	SDL_GetWindowSize(window, &screen_width, &screen_height);

	init_texture();

	time_t lastT = time(NULL);

	if (strobeCrossTalkTest)
		activeTest = std::make_shared<StrobeCrossTalk>();
	else
		activeTest = std::make_shared<PursuitCamera>();

	int frames_per_second_cnt;
	while (true)
	{
		if (!get_input())
			break;

		activeTest->draw();
		SDL_GL_SwapWindow(window);

		if (msdelay)
			usleep(msdelay * 1000);

		time_t nowT = time(NULL);
		frames_per_second_cnt++;
		if (nowT != lastT)
		{
			frames_per_second = frames_per_second_cnt;
			lastT			  = nowT;
			pixels_per_frame  = roundf((float)pixels_per_second / (float)frames_per_second);
			if (pixels_per_frame > 20)
				pixels_per_frame = 20;

			frames_per_second_cnt = 0;
		}
	}

	deinit_texture();
	TTF_CloseFont(font);
	TTF_Quit();

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
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
			case SDLK_F1:
				activeTest = std::make_shared<PursuitCamera>();
				break;
			case SDLK_F2:
				activeTest = std::make_shared<StrobeCrossTalk>();
				break;
			case SDLK_F3:
				activeTest = std::make_shared<VerticalText>();
				break;
			case SDLK_1:
				activeTest = std::make_shared<SingleColor>(1, 0, 0);
				break;
			case SDLK_2:
				activeTest = std::make_shared<SingleColor>(0, 1, 0);
				break;
			case SDLK_3:
				activeTest = std::make_shared<SingleColor>(0, 0, 1);
				break;
			case SDLK_4:
				activeTest = std::make_shared<SingleColor>(1, 1, 1);
				break;
			case SDLK_5:
				activeTest = std::make_shared<SingleColor>(0, 0, 0);
				break;
			case SDLK_F4:
				activeTest = std::make_shared<GridPattern>();
				break;
			case SDLK_F5:
				activeTest = std::make_shared<Contrast>();
				break;
			case SDLK_F6:
				activeTest = std::make_shared<MprtTest>();
				break;
#if 0
			case SDLK_q:
				msdelay++;
				break;
			case SDLK_a:
				if (msdelay > 0)
					msdelay--;
				break;
#endif

			case SDLK_e:
			{
				MprtTest* mprt = dynamic_cast<MprtTest*>(activeTest.get());
				if (mprt)
					mprt->alterCheckerBoardSize(-1);
				break;
			}
			case SDLK_r:
			{
				MprtTest* mprt = dynamic_cast<MprtTest*>(activeTest.get());
				if (mprt)
					mprt->alterCheckerBoardSize(1);
				break;
			}
			case SDLK_d:
			{
				MprtTest* mprt = dynamic_cast<MprtTest*>(activeTest.get());
				if (mprt)
					mprt->alterThickness(-1);
				break;
			}
			case SDLK_f:
			{
				MprtTest* mprt = dynamic_cast<MprtTest*>(activeTest.get());
				if (mprt)
					mprt->alterThickness(1);
				break;
			}
			case SDLK_c:
			{
				MprtTest* mprt = dynamic_cast<MprtTest*>(activeTest.get());
				if (mprt)
					mprt->alterPixelsPerFrame(-1);
				break;
			}
			case SDLK_v:
			{
				MprtTest* mprt = dynamic_cast<MprtTest*>(activeTest.get());
				if (mprt)
					mprt->alterPixelsPerFrame(1);
				break;
			}
			}
		}
	}
	return true;
}
