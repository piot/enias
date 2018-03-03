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
#include <enias/graphics_chip.h>

typedef struct render_info {
	enias_ppu* ppu;
	const uint8_t* memory;
} render_info;

static void render_callback(void* _self, uint32_t* pixels)
{
	render_info* self = (render_info*) _self;
	enias_ppu_render(self->ppu, pixels);
}

void enias_graphics_chip_render(enias_graphics_chip* self, const uint8_t* memory)
{
	enias_ppu_setup(&self->ppu, memory);

	render_info info;
	info.ppu = &self->ppu;
	info.memory = memory;
	const enias_palette_info* background_color = &self->ppu.palette[0];
	uint32_t background_value = background_color->r << 24 | background_color->g << 16 | background_color->b << 8;
	enias_render_sdl2_render(&self->render_sdl2, background_value, &info, render_callback);
}

void enias_graphics_chip_init(enias_graphics_chip* self)
{
	enias_render_sdl2_init(&self->render_sdl2, ENIAS_PPU_SCREEN_WIDTH, ENIAS_PPU_SCREEN_HEIGHT);
}
