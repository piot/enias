/*

MIT License

Copyright (c) 2017 Peter Bjorklund

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/
#include <enias/render_sdl2.h>

static SDL_Surface* create_surface(int width, int height)
{
	SDL_Surface* surface;
	Uint32 rmask, gmask, bmask, amask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif

	surface = SDL_CreateRGBSurface(0, width, height, 32, rmask, gmask, bmask, amask);
	if (surface == NULL) {
		SDL_Log("SDL_CreateRGBSurface() failed: %s", SDL_GetError());
		exit(1);
	}

	surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);

	return surface;
}

#define ENIAS_FRAME_TIME (16)

void enias_render_sdl2_init(enias_render_sdl2* self, int VIRTUAL_SCREEN_WIDTH, int VIRTUAL_SCREEN_HEIGHT)
{
	SDL_Window* window = 0;
	SDL_Surface* screen_surface = 0;

	int SCREEN_WIDTH = VIRTUAL_SCREEN_WIDTH * 3;
	int SCREEN_HEIGHT = VIRTUAL_SCREEN_HEIGHT * 3;

	SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "1");
	SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_NOPARACHUTE | SDL_INIT_AUDIO) < 0) {
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}

	window = SDL_CreateWindow("enias", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS);
	if (window == 0) {
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
	}
	SDL_RaiseWindow(window);
	screen_surface = SDL_GetWindowSurface(window);
	SDL_FillRect(screen_surface, NULL, SDL_MapRGB(screen_surface->format, 0xFF, 0xFF, 0xFF));
	SDL_Surface* virtual_screen_surface = create_surface(VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT);

	self->window = window;
	self->screen_surface = screen_surface;
	self->virtual_screen_surface = virtual_screen_surface;

	self->next_frame_tick = 0;
}

static void fill_background_color(SDL_Surface* surface, uint32_t color)
{
	SDL_FillRect(surface, 0, color);
}

static uint32_t ticks_to_sleep(enias_render_sdl2* self)
{
	uint32_t now = SDL_GetTicks();
	if (self->next_frame_tick <= now)
		return 0;
	else
		return self->next_frame_tick - now;
}

static void update_screen(SDL_Window* window, SDL_Surface* screen_surface, SDL_Surface* virtual_screen_surface)
{
	SDL_BlitScaled(virtual_screen_surface, 0, screen_surface, 0);
	SDL_UpdateWindowSurface(window);
}

void enias_render_sdl2_render(enias_render_sdl2* self, uint32_t background_color, void* userdata, enias_render_sdl2_callback callback)
{
	fill_background_color(self->virtual_screen_surface, background_color);
	SDL_LockSurface(self->virtual_screen_surface);

	uint32_t* pixels = self->virtual_screen_surface->pixels;

	callback(userdata, pixels);

	SDL_UnlockSurface(self->virtual_screen_surface);
	update_screen(self->window, self->screen_surface, self->virtual_screen_surface);

	SDL_Delay(ticks_to_sleep(self));
	self->next_frame_tick = SDL_GetTicks() + ENIAS_FRAME_TIME;
}

void enias_render_sdl2_close(enias_render_sdl2* self)
{
	SDL_DestroyWindow(self->window);
	SDL_Quit();
}
