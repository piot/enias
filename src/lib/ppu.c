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
#include <enias/ppu.h>
#include <stddef.h>

#define ENIAS_PPU_TILE_ADDRESS (0xB000)
#define ENIAS_PPU_SPRITE_ADDRESS (0xF400)
#define ENIAS_PPU_PALETTE_ADDRESS (0xF500)
#define ENIAS_PPU_NAMETABLE_ADDRESS (0xF000)

#define ENIAS_PPU_SPRITE_FLAG_PRIORITY (0x20)
#define ENIAS_PPU_SPRITE_FLAG_HFLIP (0x40)
#define ENIAS_PPU_SPRITE_FLAG_VFLIP (0x80)

static void tile_to_screen(uint32_t* surface_pixels, const enias_ppu* ppu, size_t tile_index, uint8_t screen_x, uint8_t screen_y)
{
	if (screen_y >= ENIAS_PPU_SCREEN_HEIGHT) {
		return;
	}
	uint8_t rows_to_draw = 8;
	if (screen_y + 8 >= ENIAS_PPU_SCREEN_HEIGHT) {
		rows_to_draw = ENIAS_PPU_SCREEN_HEIGHT - screen_y;
	}
	uint8_t tix = tile_index % 32;
	uint8_t tiy = tile_index / 32;
	uint16_t tmx = tix * 8;
	uint16_t tmy = tiy * 8;
	for (uint8_t ty = 0; ty < rows_to_draw; ++ty) {
		for (uint8_t tx = 0; tx < 8; ++tx) {
			uint16_t vx = tmx + tx;
			uint16_t vy = tmy + ty;
			const uint16_t chars_offset = (vy * ENIAS_PPU_SCREEN_WIDTH / 2) + (vx / 2);
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
				uint32_t target_surface_offset = (tpy * ENIAS_PPU_SCREEN_WIDTH + tpx);
				surface_pixels[target_surface_offset] = color;
			}
		}
	}
}

static void render_background_chars(uint32_t* surface_pixels, enias_ppu* ppu)
{
	for (uint8_t y = 0; y < ENIAS_PPU_SCREEN_HEIGHT / 8; ++y) {
		for (uint8_t x = 0; x < ENIAS_PPU_SCREEN_WIDTH / 8; ++x) {
			uint32_t name_table_offset = y * 32 + x;
			const enias_name_table_info* name_entry = ppu->name_table + name_table_offset;
			uint8_t tile_index = name_entry->tile_index;
			uint8_t screen_x = x * 8 - ppu->scroll_x;
			uint8_t screen_y = y * 8 - ppu->scroll_y;
			tile_to_screen(surface_pixels, ppu, tile_index, screen_x, screen_y);
		}
	}
}

static void render_sprites(uint32_t* surface_pixels, enias_ppu* ppu, uint8_t priority)
{
	for (uint8_t i = 0; i < 64; ++i) {
		const enias_sprite_info* sprite = ppu->sprites + i;
		uint8_t tile_index = sprite->tile;
		uint8_t screen_x = sprite->x;
		uint8_t screen_y = sprite->y;
		uint8_t flags = sprite->flags;

		int should_be_shown = (flags & ENIAS_PPU_SPRITE_FLAG_PRIORITY) == priority;

		if (should_be_shown && (screen_y < ENIAS_PPU_SCREEN_HEIGHT)) {
			tile_to_screen(surface_pixels, ppu, tile_index, screen_x, screen_y);
		}
	}
}

void enias_ppu_setup(enias_ppu* ppu, const uint8_t* memory)
{
	const enias_sprite_info* sprite_info = (const enias_sprite_info*) (memory + ENIAS_PPU_SPRITE_ADDRESS);
	const enias_palette_info* palette = (const enias_palette_info*) (memory + ENIAS_PPU_PALETTE_ADDRESS);
	const enias_name_table_info* name_table = (const enias_name_table_info*) (memory + ENIAS_PPU_NAMETABLE_ADDRESS);
	const uint8_t* tile_start = (memory + ENIAS_PPU_TILE_ADDRESS);

	ppu->sprites = sprite_info;
	ppu->palette = palette;
	ppu->name_table = name_table;
	ppu->chars = tile_start;
	ppu->scroll_x = memory[0xfe08];
	ppu->scroll_y = memory[0xfe09];
}

void enias_ppu_render(enias_ppu* self, uint32_t* pixels)
{
	render_sprites(pixels, self, 0);
	render_background_chars(pixels, self);
	render_sprites(pixels, self, ENIAS_PPU_SPRITE_FLAG_PRIORITY);
}
