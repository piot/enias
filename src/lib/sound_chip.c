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

int enias_sound_chip_init(enias_sound_chip* self)
{
	shout_init(&self->chip);
	enias_sound_chip_sdl2_init(self, audio_callback);

	return 0;
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

#define ENIAS_SOUND_WAVE_ADDRESS (0xF600)
#define ENIAS_SOUND_CHANNEL_ADDRESS (0xFA00)
#define ENIAS_SOUND_INSTRUMENTS_ADDRESS (0xF900)

void enias_sound_chip_update(enias_sound_chip* self, const uint8_t* memory)
{
	const shout_instrument* instruments = (const shout_instrument*) (memory + ENIAS_SOUND_INSTRUMENTS_ADDRESS);
	const enias_sound_wave* enias_waves = (const enias_sound_wave*) (memory + ENIAS_SOUND_WAVE_ADDRESS);
	const shout_channel* channels = (const shout_channel*) (memory + ENIAS_SOUND_CHANNEL_ADDRESS);

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
