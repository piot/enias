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
#ifndef shout_h
#define shout_h

#include <stddef.h>
#include <stdint.h>

#include <shout/delay.h>
#include <shout/reverb.h>

const static uint8_t SAMPLE_TYPE_16_BIT = 0x10;
const static uint8_t LOOP_TYPE_NO_LOOP = 0x00;
const static uint8_t LOOP_TYPE_FORWARD = 0x01;
const static uint8_t LOOP_TYPE_PING_PONG = 0x02;

typedef int8_t shout_char;
typedef uint16_t shout_word;
typedef uint32_t shout_dword;
typedef uint8_t shout_byte;
typedef int8_t shout_sbyte;

#define MAX_SAMPLES 64

#pragma pack(1)

typedef struct shout_wave {
	shout_dword sample_loop_start;
	shout_dword sample_loop_length;
	shout_byte volume;
	shout_sbyte fine_tune;
	shout_byte loop_type;
	shout_byte panning;
	shout_sbyte relative_note_number;
	const int16_t* samples;
	size_t sample_length;
} shout_wave;

#define MAX_INSTRUMENTS 24

typedef struct shout_instrument {
	shout_byte wave_index;
	shout_byte attack;
	shout_byte decay;
	shout_byte sustain;
	shout_byte release;
} shout_instrument;

typedef struct shout_channel {
	uint8_t instrument_index;
	uint8_t note;
	uint8_t volume_value;
	uint8_t panoration;
	uint8_t pitch;
	uint8_t fx1_mix;
	uint8_t fx2_mix;
	uint8_t low_pass;
	uint8_t high_pass;
	uint8_t q_filter;
} shout_channel;

#pragma pack()

typedef struct shout_scratch_channel {
	int id;
	uint8_t note;
	uint32_t volume;
	uint8_t volume_value;
	uint32_t sample_position;
	uint32_t loop_start;
	uint32_t loop_end;
	uint32_t loop_length;
	uint8_t effect_type;
	uint8_t effect_parameter;
	uint32_t sample_speed;
	int32_t volume_speed;
	uint8_t panoration;
	const shout_wave* wave;
} shout_scratch_channel;

#define MAX_CHANNELS 16
#define MAX_WAVES 64
typedef struct shout_chip {
	shout_instrument instruments[MAX_INSTRUMENTS];
	shout_channel channels[MAX_CHANNELS];
	shout_scratch_channel scratch_channels[MAX_CHANNELS];
	shout_wave waves[MAX_WAVES];
	uint8_t number_of_channels;
	shout_delay delays[2];
	shout_reverb reverbs[2];
} shout_chip;

void shout_init(shout_chip* self);

void shout_update(shout_chip* self, int16_t* samples, size_t size);

void shout_update_params(shout_chip* self);

#endif
