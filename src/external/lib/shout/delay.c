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
#include <shout/delay.h>

void shout_delay_init(shout_delay* self, unsigned int delay_samples)
{
	self->buffer_size = 32768;
	self->write_position = 0;
	self->delay_in_samples = delay_samples;
	for (size_t n = 0; n < self->buffer_size; ++n) {
		self->buffer[n] = 0;
	}
}

void shout_delay_update(shout_delay* self, int16_t* samples, size_t count, size_t step)
{
	unsigned int write_position = self->write_position;
	unsigned int read_position = self->write_position + (self->buffer_size - self->delay_in_samples);
	unsigned int buffer_size = self->buffer_size;
	int32_t ac;
	int32_t bc;
	const static int32_t mix_resolution = 256;
	int32_t delay_mix_factor = 80;
	int32_t source_mix_factor = mix_resolution - delay_mix_factor;

	for (size_t n = 0; n < count * 2; n += step) {
		write_position %= buffer_size;
		read_position %= buffer_size;
		ac = samples[n];
		bc = self->buffer[read_position];
		ac *= source_mix_factor;
		ac += bc * delay_mix_factor;
		ac /= mix_resolution;
		self->buffer[write_position] = ac;
		samples[n] = ac;

		write_position++;
		read_position++;
	}

	self->write_position = write_position;
}
