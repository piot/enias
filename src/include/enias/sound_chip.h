#ifndef enias_sound_chip_h
#define enias_sound_chip_h

#include <shout/shout.h>

typedef struct enias_sound_chip {
	shout_chip chip;
	uint32_t debug_frame_count;
} enias_sound_chip;

int enias_sound_chip_init(enias_sound_chip* self);
void enias_sound_chip_update(enias_sound_chip* self, const uint8_t* memory);

#endif
