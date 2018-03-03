#ifndef enias_chip_sdl2_h
#define enias_chip_sdl2_h

#include <stdint.h>

typedef void (*enias_sound_chip_callback)(void* self, int16_t* sample, int sample_count);

int enias_sound_chip_sdl2_init(void* userdata, enias_sound_chip_callback callback);

#endif
