/**
 * @file strobeCrossTalk.cpp
 * @author Slamy
 *
 * Based on the example from https://geometrian.com/programming/tutorials/texturegl/index.php
 * Currently only tested with SDL2 on Debian Testing.
 */

#include <GL/gl.h>
#include <GL/glu.h>

// Choose whether to use SDL1 or SDL2
#if 0
#include <SDL1/include/SDL.h>
#include <SDL1/include/SDL_opengl.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <assert.h>
#include <cstdio>

static SDL_Window* window;
#endif

int screen_width  = 800;
int screen_height = 600;

GLuint texture;

int textureW	 = 0;
int textureH	 = 0;
int scroll_speed = 4;

SDL_Surface* surf = nullptr;

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
			case SDLK_q:
				scroll_speed++;
				break;
			case SDLK_a:
				if (scroll_speed > 0)
					scroll_speed--;
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

static void draw(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, screen_width, screen_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, screen_width, 0, screen_height);

	int y = 0;
	int x = 0;

	int spacing_y		 = 100;
	int spacing_x		 = 140;
	static int offset_x	 = 0;
	static int frame_cnt = 0;
	int y_row			 = 0;

	for (y = spacing_y / 2; y < screen_height; y += spacing_y)
	{
		if ((frame_cnt & 1) || (y_row & 1))
		{
			for (x = -spacing_x + offset_x; x < screen_width; x += spacing_x)
			{
				drawUfo(x, y);
			}
		}

		y_row++;
	}

	frame_cnt++;

	offset_x += scroll_speed;
	if (offset_x >= spacing_x)
		offset_x = 0;

#if SDL_MAJOR_VERSION == 1
	SDL_GL_SwapBuffers();
#elif SDL_MAJOR_VERSION == 2
	SDL_GL_SwapWindow(window);
#else
#error
#endif
}

int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING | SDL_INIT_NOPARACHUTE);

	// Load the image from the file into SDL's surface representation
	surf = SDL_LoadBMP("testpattern.bmp");
	if (surf == NULL)
	{
		printf("Error: \"%s\"\n", SDL_GetError());
		return 1;
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

#if SDL_MAJOR_VERSION == 1
	SDL_WM_SetCaption("SDL and OpenGL example - Ian Mallett", NULL);
	SDL_SetVideoMode(screen_width, screen_height, 32, SDL_OPENGL);
#elif SDL_MAJOR_VERSION == 2
	window = SDL_CreateWindow("SDL and OpenGL example - Ian Mallett", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
							  screen_width, screen_height, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP);
	SDL_GLContext context = SDL_GL_CreateContext(window);
#else
#error
#endif

	glDisable(GL_DEPTH_TEST);
	// SDL_GL_SetSwapInterval(1);

	SDL_GetWindowSize(window, &screen_width, &screen_height);

	init_texture();

	while (true)
	{
		if (!get_input())
			break;
		draw();
	}

	deinit_texture();

#if SDL_MAJOR_VERSION == 1
#elif SDL_MAJOR_VERSION == 2
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
#else
#error
#endif
	SDL_Quit();

	return 0;
}
