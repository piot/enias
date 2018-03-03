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
#include <enias/sound_chip_sdl2.h>

#include <SDL2/SDL.h>

#define TYRAN_LOG_ERROR(x)

static enias_sound_chip_callback g_enias_sound_chip_callback;

static void audio_callback(void* _self, Uint8* target, int octet_length)
{
	// thunder_sound_driver* self = _self;
	// thunder_audio_buffer* sound_buffer = self->buffer;

	if ((octet_length % 4) != 0) {
		TYRAN_LOG_ERROR("ERROR!!!!!");
	}
	size_t sample_count_to_fill = octet_length / (sizeof(int16_t) * 2);
	if (sample_count_to_fill > 16 * 1024) {
		TYRAN_LOG_ERROR("ERROR!!!!!");
		return;
	}
	if ((sample_count_to_fill % 8) != 0) {
		TYRAN_LOG_ERROR("DIVISON ERR");
		return;
	}
	g_enias_sound_chip_callback(_self, (int16_t*) target, sample_count_to_fill);
}

int enias_sound_chip_sdl2_init(void* userdata, enias_sound_chip_callback callback)
{
	g_enias_sound_chip_callback = callback;
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		return 1;
	}

	SDL_AudioSpec wav_spec;

	wav_spec.callback = audio_callback;
	wav_spec.userdata = userdata;
	wav_spec.format = AUDIO_S16;
	wav_spec.samples = 2048;
	wav_spec.channels = 2;
	wav_spec.freq = 44100;

	if (SDL_OpenAudio(&wav_spec, 0) < 0) {
		fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
		return 1;
	}

	SDL_PauseAudio(0);
	return 0;
}
