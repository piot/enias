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

typedef struct enias_engine {
	zany_cpu cpu;
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

static inline uint16_t get_address_indirect(const uint8_t* memory, const uint16_t address)
{
	return memory[address] + (memory[address + 1] << 8);
}

void render(SDL_Surface* surface, const uint8_t* memory)
{
	SDL_LockSurface(surface);

	uint16_t tile_start = get_address_indirect(memory, 0xfe00);
	uint16_t sprite_info_start = get_address_indirect(memory, 0xfe02);
	uint16_t palette_info_start = get_address_indirect(memory, 0xfe04);
	uint16_t name_table_start = get_address_indirect(memory, 0xfe06);

	const enias_name_table_info* name_table = (const enias_name_table_info*) (memory + name_table_start);
	const enias_palette_info* palette = (const enias_palette_info*) (memory + palette_info_start);
	const enias_sprite_info* sprite_info = (const enias_sprite_info*) (memory + sprite_info_start);

	uint32_t* pixels = surface->pixels;

	for (uint8_t y = 0; y < VIRTUAL_SCREEN_HEIGHT / 8; ++y) {
		for (uint8_t x = 0; x < VIRTUAL_SCREEN_WIDTH / 8; ++x) {
			uint32_t name_table_offset = y * 32 + x;
			const enias_name_table_info* name_entry = &name_table[name_table_offset];
			uint8_t tile_index = name_entry->tile_index;
			uint8_t tix = tile_index % 32;
			uint8_t tiy = tile_index / 32;
			uint16_t tmx = tix * 8;
			uint16_t tmy = tiy * 8;
			for (uint8_t ty = 0; ty < 8; ++ty) {
				for (uint8_t tx = 0; tx < 8; ++tx) {
					uint16_t vx = tmx + tx;
					uint16_t vy = tmy + ty;
					const uint16_t palette_index_lookup_offset = (vy * VIRTUAL_SCREEN_WIDTH / 2) + (vx / 2);
					uint8_t palette_index = memory[tile_start + palette_index_lookup_offset];
					if (vx & 1) {
						palette_index &= 0xf;
					} else {
						palette_index >>= 4;
					}
					const enias_palette_info* palette_entry = &palette[palette_index];
					uint32_t color = palette_entry->r << 16 | palette_entry->g << 8 | palette_entry->b;

					uint8_t tpx = x * 8 + tx;
					uint8_t tpy = y * 8 + ty;
					uint32_t target_surface_offset = (tpy * VIRTUAL_SCREEN_WIDTH + tpx);
					pixels[target_surface_offset] = color;
				}
			}
		}
	}

	SDL_UnlockSurface(surface);
}

int main(int argc, char* args[])
{
	zany_load(&engine.cpu, "game.prg");

	SDL_Window* window = 0;
	SDL_Surface* screenSurface = 0;

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}

	window = SDL_CreateWindow("Enias", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS);
	if (window == 0) {
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
	}
	SDL_RaiseWindow(window);
	screenSurface = SDL_GetWindowSurface(window);
	zany_run(&engine.cpu);

	SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));

	SDL_Surface* bitSurface = create_surface(VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT);

	render(bitSurface, engine.cpu.memory);

	SDL_BlitScaled(bitSurface, 0, screenSurface, 0);

	SDL_UpdateWindowSurface(window);

	SDL_Delay(8000);

	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}
