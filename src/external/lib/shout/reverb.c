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
#include <shout/reverb.h>
#include <stdlib.h>

const static int delay_buffer_count = 10;

void ring_buffer_init(ring_buffer* self, unsigned int size)
{
	self->samples = calloc(size, sizeof(int16_t));
	self->buffer_size = size;
	self->position = 0;
}

static inline void ring_buffer_add(ring_buffer* self, int16_t a)
{
	self->samples[self->position] = a;
	self->position = (self->position + 1) % self->buffer_size;
}

void shout_reverb_init(shout_reverb* self, int offset)
{
	const unsigned int delays[] = {499, 521, 839, 1049, 1427, 7687, 7691, 7699, 7703, 7717};

	for (size_t n = 0; n < delay_buffer_count; ++n) {
		ring_buffer_init(&self->ring_buffers[n], delays[n] + offset);
	}
}

void shout_reverb_update(shout_reverb* self, int16_t* samples, size_t count, size_t step)
{
	int32_t rac;
	int32_t iac;
	const int32_t reverb_mix = 5;
	const int32_t normal_mix = 10;

	for (size_t n = 0; n < count * 2; n += step) {
		iac = samples[n];

		rac = 0;
		for (size_t d = 0; d < delay_buffer_count; ++d) {
			const ring_buffer* rb = &self->ring_buffers[d];
			int32_t sa = rb->samples[rb->position];
			rac += sa;
		}
		rac /= (delay_buffer_count);

		int32_t out = ((rac * reverb_mix + iac * normal_mix) * 2) / (reverb_mix + normal_mix);

		for (size_t d = 0; d < delay_buffer_count; ++d) {
			ring_buffer* rb = &self->ring_buffers[d];
			ring_buffer_add(rb, out);
		}

		samples[n] = out;
	}
}
