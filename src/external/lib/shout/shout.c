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
#include <math.h>
#include <shout/shout.h>
#include <stdio.h>
#include <string.h>

#ifndef tyran_mem_clear_type
#define tyran_memset_type(T, V) memset(T, V, sizeof(*T))
#define tyran_mem_clear_type(T) tyran_memset_type(T, 0);
#endif

void shout_init(shout_chip* self)
{
	tyran_mem_clear_type(self);
	for (size_t i = 0; i < MAX_CHANNELS; ++i) {
		self->scratch_channels[i].id = (int) i;
	}
	self->number_of_channels = 2;

	shout_delay_init(&self->delays[0], 20000);
	shout_delay_init(&self->delays[1], 14000);
	shout_reverb_init(&self->reverbs[0], 0);
	shout_reverb_init(&self->reverbs[1], 1181);
}

static void apply_volume(shout_scratch_channel* scratch_channel)
{
	uint32_t sample_volume = 64;

	if (scratch_channel->wave) {
		sample_volume = scratch_channel->wave->volume;
	}
	uint32_t added_volume = scratch_channel->volume_value * sample_volume;
	scratch_channel->volume = added_volume / 64;
}

static void apply_instrument(shout_chip* self, shout_scratch_channel* scratch_channel, const shout_channel* channel)
{
	const shout_instrument* instrument = &self->instruments[channel->instrument_index];
	// uint8_t sample_index;
	// sample_index = channel->instrument->sample_number[channel->note];
	uint8_t wave_index = instrument->wave_index;

	scratch_channel->wave = &self->waves[wave_index];

	if (!scratch_channel->wave) {
		return;
	}
	scratch_channel->sample_position = 0;
	int should_play = channel->note & 0x80;
	scratch_channel->panoration = channel->panoration;

	if (!should_play) {
		scratch_channel->wave = 0;
		return;
	}
	uint8_t requested_note = channel->note & 0x7f;
	uint8_t note = requested_note + scratch_channel->wave->relative_note_number;
	uint32_t multiplier = 16 * 4;
	uint32_t octave = 12 * multiplier;
	int32_t period = 10 * octave - note * multiplier - scratch_channel->wave->fine_tune / 2;
	int32_t middle = 6 * octave;
	float shifty = (middle - period) / (float) (octave);
	float f = powf(2.0f, shifty);
	uint32_t frequency = 8363.0f * f;
	scratch_channel->volume_value = scratch_channel->wave->volume;
	int is_16_bit = scratch_channel->wave->loop_type & SAMPLE_TYPE_16_BIT;
	scratch_channel->loop_start = scratch_channel->wave->sample_loop_start;

	if (is_16_bit) {
		scratch_channel->loop_start /= 2;
	}
	scratch_channel->loop_length = scratch_channel->wave->sample_loop_length;

	if (is_16_bit) {
		scratch_channel->loop_length /= 2;
	}
	scratch_channel->loop_end = scratch_channel->loop_start + scratch_channel->loop_length;

	apply_volume(scratch_channel);
	scratch_channel->sample_speed = (frequency << 10) / 44100;
}

void shout_update_params(shout_chip* self)
{
	for (size_t channel_index = 0; channel_index < self->number_of_channels; ++channel_index) {
		const shout_channel* channel = &self->channels[channel_index];
		shout_scratch_channel* scratch_channel = &self->scratch_channels[channel_index];

		if (scratch_channel->note != channel->note) {
			apply_instrument(self, scratch_channel, channel);
			scratch_channel->note = channel->note;
		}
	}
}

void shout_update(shout_chip* self, int16_t* samples, size_t sample_count)
{
	int debug_this_frame = 0;
	int16_t max_left = 0;

	for (size_t sample_index = 0; sample_index < sample_count; sample_index++) {
		debug_this_frame = 0;
		int32_t l = 0;
		int32_t r = 0;
		for (size_t channel_index = 0; channel_index < self->number_of_channels; ++channel_index) {
			shout_scratch_channel* scratch_channel = &self->scratch_channels[channel_index];

			if (!scratch_channel->wave) {
				continue;
			}

			scratch_channel->sample_position += scratch_channel->sample_speed;

			if (scratch_channel->wave) {
				uint8_t loop_type = scratch_channel->wave->loop_type & 0x3;
				uint32_t sample_data_index = scratch_channel->sample_position >> 10;

				if (loop_type == LOOP_TYPE_NO_LOOP) {
					if (sample_data_index > scratch_channel->wave->sample_length) {
						scratch_channel->wave = 0;
						continue;
					}
				} else {
					if (sample_data_index >= scratch_channel->loop_end) {
						uint32_t rest = sample_data_index - scratch_channel->loop_end;
						uint32_t loop_offset = (rest % scratch_channel->loop_length);

						if (loop_type == LOOP_TYPE_FORWARD) {
							sample_data_index = scratch_channel->loop_start + loop_offset;
						} else if (loop_type == LOOP_TYPE_PING_PONG) {
							uint32_t loop_count = rest / scratch_channel->loop_length;

							if (loop_count % 2) {
								sample_data_index = scratch_channel->loop_start + loop_offset;
							} else {
								sample_data_index = scratch_channel->loop_end - loop_offset;
							}
						} else {
							sample_data_index = 0;
						}
					}
				}
				int32_t v = scratch_channel->wave->samples[sample_data_index];
				uint8_t pan = scratch_channel->panoration >> 2;
				int32_t left_factor = 63 - pan;
				int32_t right_factor = pan;
				int32_t with_volume = v * (int32_t) scratch_channel->volume;
				int32_t local_l = (with_volume * left_factor);
				int32_t local_r = (with_volume * right_factor);
				l += local_l;
				r += local_r;
			}
		}
		const int32_t divisor = 64 * 32;
		l /= self->number_of_channels * divisor;
		r /= self->number_of_channels * divisor;
		uint16_t abs = l;

		if (l < 0) {
			abs = -l;
		}

		if (abs > max_left) {
			max_left = abs;
		}
		samples[sample_index * 2] = l;
		samples[sample_index * 2 + 1] = r;
	}

#if 1
	shout_delay_update(&self->delays[0], samples, sample_count, 2);
	shout_delay_update(&self->delays[1], samples + 1, sample_count, 2);
	shout_reverb_update(&self->reverbs[0], samples, sample_count, 2);
	shout_reverb_update(&self->reverbs[1], samples + 1, sample_count, 2);
#endif
}
