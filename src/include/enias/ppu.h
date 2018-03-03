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
#ifndef enias_ppu_h
#define enias_ppu_h

#include <stdint.h>

#define ENIAS_PPU_SCREEN_WIDTH (256)
#define ENIAS_PPU_SCREEN_HEIGHT (224)

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
	uint8_t scroll_x;
	uint8_t scroll_y;
} enias_ppu;

void enias_ppu_setup(enias_ppu* ppu, const uint8_t* memory);
void enias_ppu_render(enias_ppu* ppu, uint32_t* pixels);

#endif
