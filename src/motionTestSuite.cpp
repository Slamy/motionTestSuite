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
#include <list>
#include <numeric>
#include <sys/resource.h>
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
int screen_height = 900;

GLuint texture_ufo;

int moving_target_width	 = 0;
int moving_target_height = 0;
int pixels_per_frame	 = 6;
int pixels_per_second	 = 960;

int frame_cnt				  = 0;
int out_of_sync_cnt			  = 0;
double frames_per_second	  = 0;
double frame_times_avg		  = 0;
static bool draw_jitter_graph = true;
static int frames_cnt		  = 0;
static int ignore_frametime	  = 10;

std::list<double> last_frame_times;

SDL_Surface* surface_target = nullptr;

TTF_Font* font = nullptr;

std::shared_ptr<MotionTest> activeTest;

namespace po = boost::program_options;

static bool get_input(void);
static int msdelay			= 0;
static uint32_t speed_index = 5;

// Same speeds as with https://www.testufo.com/
static std::vector<int> possible_pixels_per_second{
	0, 120, 240, 480, 720, 960, 1200, 1440, 1920, 2560,
};

static void init_texture(void)
{
	assert(surface_target);

	glGenTextures(1, &texture_ufo);
	glBindTexture(GL_TEXTURE_2D, texture_ufo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface_target->w, surface_target->h, 0, GL_BGRA, GL_UNSIGNED_BYTE,
				 surface_target->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	moving_target_width	 = surface_target->w;
	moving_target_height = surface_target->h;
	printf("Texture %d %d\n", moving_target_width, moving_target_height);

	// Unload SDL's copy of the data; we don't need it anymore because OpenGL now stores it in the texture.
	SDL_FreeSurface(surface_target);
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

	int stride = surfaceMessage->pitch / surfaceMessage->format->BytesPerPixel;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, stride, surfaceMessage->h, 0, GL_BGRA, GL_UNSIGNED_BYTE,
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
	glVertex2f(0, moving_target_height);

	glTexCoord2f(0, 1);
	glVertex2f(0, 0);

	glTexCoord2f(1, 1);
	glVertex2f(moving_target_width, 0);

	glTexCoord2f(1, 0);
	glVertex2f(moving_target_width, moving_target_height);
	glEnd();

	// Tell OpenGL that all subsequent drawing operations should NOT try to use the current 2D texture
	glDisable(GL_TEXTURE_2D);
}

void drawJitterGraph()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(30, 60, 0);

	// Draw horizontal green lines.
	// One in the center and two at the "limits"
	// We define the "limit" as half of the the available head room

	static constexpr int kDistanceBetweenSamples = 3;
	static constexpr int kSampleWidth			 = 5;
	static constexpr int kSampleXSteps			 = kSampleWidth + kDistanceBetweenSamples;
	static constexpr double kHeadRoomFactor		 = 0.6;
	double head_room_ms							 = frame_times_avg * kHeadRoomFactor;
	static constexpr double kHeadRoomPixelSize	 = 30;

	glColor3f(0, 1, 0);

	glBegin(GL_LINES);
	glVertex2f(-10, 0);
	glVertex2f(10 + last_frame_times.size() * kSampleXSteps, 0);

	glVertex2f(-10, kHeadRoomPixelSize);
	glVertex2f(10 + last_frame_times.size() * kSampleXSteps, kHeadRoomPixelSize);

	glVertex2f(-10, -kHeadRoomPixelSize);
	glVertex2f(10 + last_frame_times.size() * kSampleXSteps, -kHeadRoomPixelSize);
	glEnd();

	glColor3f(1, 0, 0);

	glBegin(GL_QUADS);
	int index = 0;
	for (const auto& val : last_frame_times)
	{
		int offset				  = index * kSampleXSteps;
		double frame_time_diff_ms = val - frame_times_avg;
		double frame_time_pixels  = frame_time_diff_ms * kHeadRoomPixelSize / head_room_ms;
		if (abs(frame_time_pixels) < 1)
		{
			frame_time_pixels = -1;
		}
		glVertex2f(offset + 0, 0);
		glVertex2f(offset + kSampleWidth, 0);
		glVertex2f(offset + kSampleWidth, frame_time_pixels);
		glVertex2f(offset + 0, frame_time_pixels);

		index++;
	}
	glEnd();
}

int main(int argc, char* argv[])
{
	int opt;
	po::options_description desc("Allowed options");
	bool disable_vsnc		 = false;
	bool activateFullScreen	 = false;
	bool strobeCrossTalkTest = false;
	int display				 = 0;

	// clang-format off
	desc.add_options()
	    ("help,h", "produce help message")
	    ("novsync", po::bool_switch(&disable_vsnc)->default_value(false), "Disables VSync")
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

	if (sched_setscheduler(0, SCHED_RR, &sp) == -1)
	{
		std::cout << "sched_setscheduler failed\n";
		return 1;
	}

	// Initialize SDL Context
	int sdlWindowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
	if (activateFullScreen)
	{
		sdlWindowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	}

	SDL_Init(SDL_INIT_VIDEO);

	if (TTF_Init() == -1)
	{
		printf("Unable to initialize SDL_ttf: %s \n", TTF_GetError());
		return 1;
	}

	// Read the font
	const char* fontpath = "font.ttf";
	font				 = TTF_OpenFont(fontpath, 20);
	if (font == NULL)
	{
		printf("Unable to load font %s: %s \n", fontpath, TTF_GetError());
		return 1;
	}

	// Load the test pattern image from the file into SDL's surface representation
	surface_target = SDL_LoadBMP("testpattern.bmp");
	if (surface_target == NULL)
	{
		printf("Error: \"%s\"\n", SDL_GetError());
		return 1;
	}

	if (!activateFullScreen)
	{
		// In Windowed mode, the Compositor is allowed to stay to avoid switching back and forth.
		// For Full Screen we could do this as well, to avoid a frame drop after 1 second.
		// But this leads to possible frame stutter.
		SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	window = SDL_CreateWindow("Slamy's SDL Motion Test Suite", SDL_WINDOWPOS_CENTERED_DISPLAY(display),
							  SDL_WINDOWPOS_CENTERED_DISPLAY(display), screen_width, screen_height, sdlWindowFlags);

	SDL_GLContext context = SDL_GL_CreateContext(window);

	glDisable(GL_DEPTH_TEST);
	SDL_GL_SetSwapInterval(disable_vsnc ? 0 : 1);
	SDL_GetWindowSize(window, &screen_width, &screen_height);

	init_texture();

	if (strobeCrossTalkTest)
		activeTest = std::make_shared<StrobeCrossTalk>();
	else
		activeTest = std::make_shared<PursuitCamera>();

	double last_frame = 0;

	bool running{true};

	std::cout << "PID: " << getpid() << std::endl;

	while (running)
	{
		if (!get_input())
			running = false;

		activeTest->draw();

		if (out_of_sync_cnt)
		{
			drawText(30, 40, "OUT OF SYNC!!!");
			out_of_sync_cnt--;
		}

		if (last_frame_times.size() > 2 && activeTest->MustBeJitterFree() && draw_jitter_graph)
		{
			drawJitterGraph();
		}

		SDL_GL_SwapWindow(window);

		if (msdelay)
			usleep(msdelay * 1000);

		frames_cnt++;

		struct timespec tp;
		clock_gettime(CLOCK_MONOTONIC, &tp);
		double now	= (tp.tv_nsec / 1000000.0 + tp.tv_sec * 1000.0);
		double diff = now - last_frame;

		if (ignore_frametime)
		{
			ignore_frametime--;
		}
		else
		{
			last_frame_times.push_back(diff);
			if (last_frame_times.size() > 100)
				last_frame_times.pop_front();
		}

		frame_times_avg =
			std::accumulate(std::begin(last_frame_times), std::end(last_frame_times), 0.0) / last_frame_times.size();

		if (last_frame_times.size() > 15)
		{
			frames_per_second = 1000.0 / frame_times_avg;
			pixels_per_second = possible_pixels_per_second.at(speed_index);
			pixels_per_frame  = roundf((float)pixels_per_second / (float)frames_per_second);
			if (pixels_per_frame > 20)
				pixels_per_frame = 20;

			static constexpr double kHeadRoomFactor = 0.6;
			double head_room_ms						= frame_times_avg * kHeadRoomFactor;
			if (abs(frame_times_avg - diff) > head_room_ms)
			{
				std::cout << "Frame " << frames_cnt << " out of sync: " << diff << " !~= " << frame_times_avg
						  << std::endl;

				if (frames_cnt > frames_per_second * 3 && activeTest->MustBeJitterFree())
				{
					out_of_sync_cnt = frames_per_second;
				}
			}
		}

		last_frame = now;
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
		case SDL_WINDOWEVENT:
			// printf("SDL_WINDOWEVENT %d\n", event.window.event);

			if (event.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				screen_width  = event.window.data1;
				screen_height = event.window.data2;
				// printf("Resized to %d %d\n", event.window.data1, event.window.data2);
				activeTest->screenResized();
			}

			switch (event.window.event)
			{
			case SDL_WINDOWEVENT_EXPOSED:
			case SDL_WINDOWEVENT_SIZE_CHANGED:
			case SDL_WINDOWEVENT_RESIZED:
			case SDL_WINDOWEVENT_MAXIMIZED:
			case SDL_WINDOWEVENT_MINIMIZED:
			case SDL_WINDOWEVENT_RESTORED:
				last_frame_times.clear();
				ignore_frametime = 20;
				out_of_sync_cnt	 = 0;
				break;
			}

			break;
		case SDL_QUIT:
			return false; // The little X in the window got pressed
		case SDL_KEYDOWN:
		{
			SingleColor* singleColor = dynamic_cast<SingleColor*>(activeTest.get());
			if (singleColor)
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_1:
					singleColor->setColor(1, 0, 0);
					break;
				case SDLK_2:
					singleColor->setColor(0, 1, 0);
					break;
				case SDLK_3:
					singleColor->setColor(0, 0, 1);
					break;
				case SDLK_4:
					singleColor->setColor(1, 1, 1);
					break;
				case SDLK_5:
					singleColor->setColor(0, 0, 0);
					break;
				}
			}

			GridPattern* gridPattern = dynamic_cast<GridPattern*>(activeTest.get());
			if (gridPattern)
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_1:
					gridPattern->setColor(1, 1, 1);
					break;
				case SDLK_2:
					gridPattern->setColor(1, 1, 0);
					break;
				case SDLK_3:
					gridPattern->setColor(0, 1, 1);
					break;
				case SDLK_4:
					gridPattern->setColor(1, 0, 1);
					break;
				case SDLK_5:
					gridPattern->setColor(1, 0, 0);
					break;
				case SDLK_6:
					gridPattern->setColor(0, 1, 0);
					break;
				case SDLK_7:
					gridPattern->setColor(0, 0, 1);
					break;
				case SDLK_8:
					gridPattern->setColor(0, 0, 0);
					break;
				}
			}

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
			case SDLK_F4:
				activeTest = std::make_shared<GridPattern>(false);
				break;
			case SDLK_F5:
				activeTest = std::make_shared<GridPattern>(true);
				break;
			case SDLK_F6:
				activeTest = std::make_shared<Contrast>(0);
				break;
			case SDLK_F7:
				activeTest = std::make_shared<Contrast>(32);
				break;
			case SDLK_F8:
				activeTest = std::make_shared<MprtTest>();
				break;
			case SDLK_F9:
				activeTest = std::make_shared<SingleColor>(1, 1, 1);
				break;

			case SDLK_q:
				if (speed_index < possible_pixels_per_second.size() - 1)
					speed_index++;
				break;
			case SDLK_a:
				if (speed_index)
					speed_index--;
				break;

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
			case SDLK_j:
			{
				draw_jitter_graph = !draw_jitter_graph;
				break;
			}
			}
		}
		}
	}
	return true;
}
