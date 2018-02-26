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
#include <zany/cpu.h>
#include <zany/loader.h>
#include <zany/run.h>

const int VIRTUAL_SCREEN_WIDTH = 256;
const int VIRTUAL_SCREEN_HEIGHT = 224;

const int SCREEN_WIDTH = VIRTUAL_SCREEN_WIDTH * 3;
const int SCREEN_HEIGHT = VIRTUAL_SCREEN_HEIGHT * 3;

#pragma pack(1)
typedef struct enias_sprite_info {
	uint8_t x;
	uint8_t y;
	uint8_t tile;
	uint8_t flags;
} enias_sprite_info;
typedef struct enias_palette_info {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} enias_palette_info;

typedef struct enias_name_table_info {
	uint8_t tile_index;
} enias_name_table_info;

#pragma pack()

typedef struct enias_ppu {
	const enias_name_table_info* name_table;
	const enias_palette_info* palette;
	const enias_sprite_info* sprites;
	const uint8_t* chars;
} enias_ppu;

typedef struct enias_gamepad {
	uint8_t normal;
	uint8_t extended;
} enias_gamepad;

typedef struct enias_input {
	enias_gamepad gamepads[2];
} enias_input;

typedef struct enias_engine {
	zany_cpu cpu;
	enias_ppu ppu;
	enias_input input;
	SDL_Surface* screen_surface;
	SDL_Window* window;
} enias_engine;

enias_engine engine;

SDL_Surface* create_surface(int width, int height)
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

static void tile_to_screen(uint32_t* surface_pixels, const enias_ppu* ppu, size_t tile_index, uint8_t screen_x, uint8_t screen_y)
{
	uint8_t tix = tile_index % 32;
	uint8_t tiy = tile_index / 32;
	uint16_t tmx = tix * 8;
	uint16_t tmy = tiy * 8;
	for (uint8_t ty = 0; ty < 8; ++ty) {
		for (uint8_t tx = 0; tx < 8; ++tx) {
			uint16_t vx = tmx + tx;
			uint16_t vy = tmy + ty;
			const uint16_t chars_offset = (vy * VIRTUAL_SCREEN_WIDTH / 2) + (vx / 2);
			uint8_t chars_pixel = *(ppu->chars + chars_offset);
			if (vx & 1) {
				chars_pixel &= 0xf;
			} else {
				chars_pixel >>= 4;
			}
			if (chars_pixel) {
				const enias_palette_info* palette_entry = &ppu->palette[chars_pixel];
				uint32_t color = palette_entry->r << 16 | palette_entry->g << 8 | palette_entry->b;

				uint8_t tpx = screen_x + tx;
				uint8_t tpy = screen_y + ty;
				uint32_t target_surface_offset = (tpy * VIRTUAL_SCREEN_WIDTH + tpx);
				surface_pixels[target_surface_offset] = color;
			}
		}
	}
}

static inline const uint8_t* get_address_indirect(const uint8_t* memory, const uint16_t address)
{
	uint16_t memory_address = memory[address] + (memory[address + 1] << 8);
	return memory + memory_address;
}

static void setup_ppu(enias_ppu* ppu, const uint8_t* memory)
{
	const enias_sprite_info* sprite_info = (const enias_sprite_info*) get_address_indirect(memory, 0xfe02);
	const enias_palette_info* palette = (const enias_palette_info*) get_address_indirect(memory, 0xfe04);
	const enias_name_table_info* name_table = (const enias_name_table_info*) get_address_indirect(memory, 0xfe06);
	const uint8_t* tile_start = get_address_indirect(memory, 0xfe00);

	ppu->sprites = sprite_info;
	ppu->palette = palette;
	ppu->name_table = name_table;
	ppu->chars = tile_start;
}

static void render_background_chars(uint32_t* surface_pixels, enias_ppu* ppu)
{
	for (uint8_t y = 0; y < VIRTUAL_SCREEN_HEIGHT / 8; ++y) {
		for (uint8_t x = 0; x < VIRTUAL_SCREEN_WIDTH / 8; ++x) {
			uint32_t name_table_offset = y * 32 + x;
			const enias_name_table_info* name_entry = ppu->name_table + name_table_offset;
			uint8_t tile_index = name_entry->tile_index;
			uint8_t screen_x = x * 8;
			uint8_t screen_y = y * 8;
			tile_to_screen(surface_pixels, ppu, tile_index, screen_x, screen_y);
		}
	}
}

static void render_sprites(uint32_t* surface_pixels, enias_ppu* ppu)
{
	for (uint8_t i = 0; i < 64; ++i) {
		const enias_sprite_info* sprite = ppu->sprites + i;
		uint8_t tile_index = sprite->tile;
		uint8_t screen_x = sprite->x;
		uint8_t screen_y = sprite->y;
		if (screen_y < VIRTUAL_SCREEN_HEIGHT && screen_x < VIRTUAL_SCREEN_WIDTH) {
			tile_to_screen(surface_pixels, ppu, tile_index, screen_x, screen_y);
		}
	}
}

void fill_background_color(SDL_Surface* surface, const enias_palette_info* color)
{
	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, color->r, color->g, color->b));
}

void render(SDL_Surface* surface, enias_ppu* ppu, const uint8_t* memory)
{
	fill_background_color(surface, &ppu->palette[0]);
	SDL_LockSurface(surface);

	uint32_t* pixels = surface->pixels;

	render_background_chars(pixels, ppu);
	render_sprites(pixels, ppu);

	SDL_UnlockSurface(surface);
}

static void update_screen(SDL_Window* window, SDL_Surface* screen_surface, SDL_Surface* virtual_screen_surface)
{
	SDL_BlitScaled(virtual_screen_surface, 0, screen_surface, 0);
	SDL_UpdateWindowSurface(window);
}

#define GAMEPAD_A (0x80)
#define GAMEPAD_B (0x40)
#define GAMEPAD_SELECT (0x20)
#define GAMEPAD_START (0x10)
#define GAMEPAD_UP (0x08)
#define GAMEPAD_DOWN (0x04)
#define GAMEPAD_LEFT (0x02)
#define GAMEPAD_RIGHT (0x01)

static uint8_t get_gamepad_mask(SDL_Keycode code)
{
	switch (code) {
		case SDLK_DOWN:
			return GAMEPAD_DOWN;
		case SDLK_UP:
			return GAMEPAD_UP;
		case SDLK_LEFT:
			return GAMEPAD_LEFT;
		case SDLK_RIGHT:
			return GAMEPAD_RIGHT;
	}

	return 0;
}

static void handle_key(enias_input* input, const SDL_KeyboardEvent* event, int on)
{
	uint8_t mask = get_gamepad_mask(event->keysym.sym);
	enias_gamepad* gamepad = &input->gamepads[0];
	if (on) {
		gamepad->normal |= mask;
	} else {
		gamepad->normal &= ~mask;
	}
	if (mask != 0) {
		//	printf("Gamepad %02X\n", gamepad->normal);
	}
}

static void handle_key_up(enias_input* input, const SDL_KeyboardEvent* event)
{
	handle_key(input, event, 0);
}

static void handle_key_down(enias_input* input, const SDL_KeyboardEvent* event)
{
	handle_key(input, event, 1);
}

static int check_sdl_events(enias_input* input)
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
					handle_key_down(input, &event.key);
				}
				break;
			case SDL_KEYUP:
				handle_key_up(input, &event.key);
				break;
		}
	}

	return quit;
}

static void set_input_to_memory(const enias_input* input, uint8_t* memory)
{
	for (size_t i = 0; i < 2; ++i) {
		memory[0xff00 + i * 2] = input->gamepads[i].normal;
		memory[0xff00 + i * 2 + 1] = input->gamepads[i].extended;
	}
}

int main(int argc, char* argv[])
{
	if (argc < 2) {
		printf("\nUsage: enias prg-file\n");
		return 0;
	}
	zany_cpu_init(&engine.cpu);
	zany_load(&engine.cpu, argv[1]);

	SDL_Window* window = 0;
	SDL_Surface* screenSurface = 0;
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}

	window = SDL_CreateWindow("Enias", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS);
	if (window == 0) {
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
	}
	SDL_RaiseWindow(window);
	engine.window = window;
	screenSurface = SDL_GetWindowSurface(window);
	SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
	engine.screen_surface = screenSurface;
	SDL_Surface* virtual_screen_surface = create_surface(VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT);
	int quit = 0;

	while (!quit) {
		quit = check_sdl_events(&engine.input);
		set_input_to_memory(&engine.input, engine.cpu.memory);
		zany_cpu_set_entry(&engine.cpu);
		int error_code = zany_run(&engine.cpu);
		if (error_code) {
			printf("ERR: cpu error code:%d\n", error_code);
			return error_code;
		}
		setup_ppu(&engine.ppu, engine.cpu.memory);

		render(virtual_screen_surface, &engine.ppu, engine.cpu.memory);
		update_screen(engine.window, engine.screen_surface, virtual_screen_surface);
		SDL_Delay(12);
	}

	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}
