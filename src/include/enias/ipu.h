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
#ifndef enias_ipu_h
#define enias_ipu_h

#include <stdint.h>

#define ENIAS_IPU_GAMEPAD_A (0x80)
#define ENIAS_IPU_GAMEPAD_B (0x40)
#define ENIAS_IPU_GAMEPAD_SELECT (0x20)
#define ENIAS_IPU_GAMEPAD_START (0x10)
#define ENIAS_IPU_GAMEPAD_UP (0x08)
#define ENIAS_IPU_GAMEPAD_DOWN (0x04)
#define ENIAS_IPU_GAMEPAD_LEFT (0x02)
#define ENIAS_IPU_GAMEPAD_RIGHT (0x01)

typedef struct enias_ipu_gamepad {
	uint8_t normal;
	uint8_t extended;
} enias_ipu_gamepad;

typedef struct enias_ipu {
	uint8_t keyboard_char;
	enias_ipu_gamepad gamepads[2];
} enias_ipu;

void enias_ipu_update(enias_ipu* self, uint8_t* memory);

#endif
