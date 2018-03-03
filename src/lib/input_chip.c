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
#include <SDL2/SDL.h>
#include <enias/input_chip.h>

static uint8_t get_gamepad_mask(SDL_Keycode code)
{
	switch (code) {
		case SDLK_DOWN:
			return ENIAS_IPU_GAMEPAD_DOWN;
		case SDLK_UP:
			return ENIAS_IPU_GAMEPAD_UP;
		case SDLK_LEFT:
			return ENIAS_IPU_GAMEPAD_LEFT;
		case SDLK_RIGHT:
			return ENIAS_IPU_GAMEPAD_RIGHT;
		case SDLK_z:
			return ENIAS_IPU_GAMEPAD_A;
		case SDLK_x:
			return ENIAS_IPU_GAMEPAD_B;
	}

	return 0;
}

static void handle_key(enias_ipu* ipu, const SDL_KeyboardEvent* event, int on)
{
	uint8_t mask = get_gamepad_mask(event->keysym.sym);
	enias_ipu_gamepad* gamepad = &ipu->gamepads[0];
	if (on) {
		gamepad->normal |= mask;
	} else {
		gamepad->normal &= ~mask;
	}
	if (mask != 0) {
		// printf("Gamepad %02X\n", gamepad->normal);
	}
}

static void handle_key_up(enias_ipu* ipu, const SDL_KeyboardEvent* event)
{
	handle_key(ipu, event, 0);
}

static void handle_key_down(enias_ipu* ipu, const SDL_KeyboardEvent* event)
{
	handle_key(ipu, event, 1);
}

static int check_sdl_events(enias_ipu* ipu)
{
	SDL_Event event;
	int quit = 0;

	if (SDL_PollEvent(&event)) {

		switch (event.type) {
			case SDL_QUIT:
				quit = 1;
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE) {
					quit = 1;
				} else {
					handle_key_down(ipu, &event.key);
				}
				break;
			case SDL_KEYUP:
				handle_key_up(ipu, &event.key);
				break;
		}
	}

	return quit;
}

void enias_input_chip_init(enias_input_chip* self)
{
}

int enias_input_chip_update(enias_input_chip* self, uint8_t* target_memory)
{
	int quit = check_sdl_events(&self->ipu);
	enias_ipu_update(&self->ipu, target_memory);
	return quit;
}
