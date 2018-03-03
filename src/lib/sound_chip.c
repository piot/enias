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
#include <enias/sound_chip.h>
#include <enias/sound_chip_sdl2.h>

#include <stdio.h>
#include <string.h>

static void audio_callback(void* _self, int16_t* sample, int sample_count)
{
	enias_sound_chip* self = (enias_sound_chip*) _self;
	shout_update(&self->chip, sample, sample_count);
}

uint8_t sine_raw[] = {0x00, 0x00, 0x6b, 0x06, 0xcc, 0x0c, 0x29, 0x13, 0x63, 0x19, 0x99, 0x1f, 0x98, 0x25, 0x85, 0x2b, 0x3b, 0x31, 0xbf, 0x36, 0x16, 0x3c, 0x22, 0x41, 0xfc, 0x45, 0x81, 0x4a, 0xc9, 0x4e, 0xb3, 0x52, 0x58, 0x56,
					  0x9c, 0x59, 0x8b, 0x5c, 0x1c, 0x5f, 0x4b, 0x61, 0x1c, 0x63, 0x86, 0x64, 0x8d, 0x65, 0x2e, 0x66, 0x65, 0x66, 0x38, 0x66, 0xa6, 0x65, 0xa7, 0x64, 0x4d, 0x63, 0x82, 0x61, 0x63, 0x5f, 0xd7, 0x5c, 0xfa, 0x59,
					  0xb7, 0x56, 0x26, 0x53, 0x3c, 0x4f, 0x02, 0x4b, 0x83, 0x46, 0xb3, 0x41, 0xab, 0x3c, 0x5e, 0x37, 0xe0, 0x31, 0x28, 0x2c, 0x4f, 0x26, 0x40, 0x20, 0x1f, 0x1a, 0xde, 0x13, 0x84, 0x0d, 0x29, 0x07, 0xb6, 0x00,
					  0x54, 0xfa, 0xe8, 0xf3, 0x96, 0xed, 0x48, 0xe7, 0x24, 0xe1, 0x0b, 0xdb, 0x2c, 0xd5, 0x67, 0xcf, 0xdd, 0xc9, 0x86, 0xc4, 0x69, 0xbf, 0x91, 0xba, 0xfd, 0xb5, 0xb3, 0xb1, 0xb7, 0xad, 0x0f, 0xaa, 0xbf, 0xa6,
					  0xc4, 0xa3, 0x2d, 0xa1, 0xed, 0x9e, 0x14, 0x9d, 0xa0, 0x9b, 0x8a, 0x9a, 0xe0, 0x99, 0x9d, 0x99, 0xbb, 0x99, 0x47, 0x9a, 0x35, 0x9b, 0x88, 0x9c, 0x44, 0x9e, 0x5c, 0xa0, 0xd7, 0xa2, 0xb2, 0xa5, 0xe2, 0xa8,
					  0x6f, 0xac, 0x50, 0xb0, 0x7b, 0xb4, 0xfd, 0xb8, 0xb7, 0xbd, 0xc5, 0xc2, 0xff, 0xc7, 0x84, 0xcd, 0x2a, 0xd3, 0x0a, 0xd9, 0x0a, 0xdf, 0x2e, 0xe5, 0x6d, 0xeb, 0xc0, 0xf1, 0x21, 0xf8, 0x89, 0xfe};

static void temp_setup(shout_chip* self)
{
	shout_wave* wave = &self->waves[0];
	uint16_t sample_count = sizeof(sine_raw) / 2;
	wave->samples = (const int16_t*) sine_raw;
	wave->sample_length = sample_count;
	wave->loop_type = LOOP_TYPE_FORWARD | SAMPLE_TYPE_16_BIT;
	wave->sample_loop_start = 0;
	wave->sample_loop_length = sample_count;
	wave->relative_note_number = 0;
	wave->fine_tune = 0;
	wave->volume = 32;

	shout_instrument* instrument = &self->instruments[0];
	instrument->wave_index = 0;

	for (size_t i = 0; i < 8; ++i) {
		shout_channel* channel = &self->channels[i];
		channel->instrument_index = 0;

		channel->panoration = 127;
		channel->volume_value = 32;
		channel->note = 0;
	}
}

int enias_sound_chip_init(enias_sound_chip* self)
{
	shout_init(&self->chip);
	temp_setup(&self->chip);
	enias_sound_chip_sdl2_init(self, audio_callback);

	return 0;
}

static inline const uint8_t* get_address_indirect(const uint8_t* memory, const uint16_t address)
{
	uint16_t memory_address = memory[address] + (memory[address + 1] << 8);
	return memory + memory_address;
}

#pragma pack(1)
typedef struct enias_sound_wave {
	uint8_t sample_lo;
	uint8_t sample_hi;
	uint8_t sample_length_lo;
	uint8_t sample_length_hi;
	uint8_t loop_type;
	uint8_t sample_loop_start_lo;
	uint8_t sample_loop_start_hi;
	uint8_t sample_loop_length_lo;
	uint8_t sample_loop_length_hi;
	uint8_t relative_note_number;
} enias_sound_wave;
#pragma pack()

void enias_sound_chip_update(enias_sound_chip* self, const uint8_t* memory)
{
	const shout_instrument* instruments = (const shout_instrument*) get_address_indirect(memory, 0xfe10);
	const enias_sound_wave* enias_waves = (const enias_sound_wave*) get_address_indirect(memory, 0xfe12);
	const shout_channel* channels = (const shout_channel*) get_address_indirect(memory, 0xfe14);

	shout_chip* chip = &self->chip;
	memcpy(chip->instruments, instruments, sizeof(shout_instrument) * MAX_INSTRUMENTS);
	memcpy(chip->channels, channels, sizeof(shout_channel) * MAX_CHANNELS);

	for (size_t wave_index = 0; wave_index < MAX_WAVES; ++wave_index) {
		shout_wave* wave = &chip->waves[wave_index];
		const enias_sound_wave* enias_wave = &enias_waves[wave_index];
		uint16_t sample_start_address = (enias_wave->sample_hi << 8) + enias_wave->sample_lo;
		wave->samples = (const int16_t*) (memory + sample_start_address);
		wave->sample_length = (enias_wave->sample_length_hi << 8) + enias_wave->sample_length_lo;
		wave->sample_loop_start = (enias_wave->sample_loop_start_hi << 8) + enias_wave->sample_loop_start_lo;
		wave->sample_loop_length = (enias_wave->sample_loop_length_hi << 8) + enias_wave->sample_loop_length_lo;
		wave->fine_tune = 0;
		wave->volume = 32;
		wave->panning = 127;
		wave->loop_type = enias_wave->loop_type;
		wave->relative_note_number = enias_wave->relative_note_number;
	}

	shout_update_params(&self->chip);
}
