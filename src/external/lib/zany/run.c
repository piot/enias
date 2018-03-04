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
#include "opcode.h"
#include <stdlib.h>
#include <zany/cpu.h>
#include <zany/run.h>

#include <stdint.h>
#include <stdio.h>

#include <time.h>

// #define ZANY_DEBUG_OPCODE 1

#define FLAG_NEGATIVE 0x80
#define FLAG_OVERFLOW 0x40
#define FLAG_ZERO 0x02
#define FLAG_CARRY 0x01

static inline void set_flag(zany_cpu* cpu, uint8_t flag, int set)
{
	if (set) {
		cpu->sr |= flag;
	} else {
		cpu->sr &= ~flag;
	}
}

static inline void set_flags(zany_cpu* cpu, uint8_t val)
{
	set_flag(cpu, FLAG_ZERO, !val);
	set_flag(cpu, FLAG_NEGATIVE, val & 0x80);
}

static inline uint8_t get_flag(zany_cpu* cpu, uint8_t flag)
{
	return (cpu->sr & flag) != 0;
}

#define TYRAN_LOG_WARN(...) printf(__VA_ARGS__)
#define TYRAN_LOG_INFO(...)                                                                                                                                                                                                                                    \
	printf(__VA_ARGS__);                                                                                                                                                                                                                                       \
	printf("\n");

#define READ_OCTET(cpu) (cpu->memory[cpu->pc++])
#define ABSOLUTE_MEMORY_OFFSET(low, high, offset) (uint16_t)((uint16_t) offset + (uint16_t) low + ((uint16_t) high << 8))
#define GET_FROM_MEMORY_OFFSET(cpu, low, high, offset) (cpu->memory[ABSOLUTE_MEMORY_OFFSET(low, high, offset)])
#define GET_INDEXED_INDIRECT(cpu, addr, offset) GET_FROM_MEMORY_OFFSET(cpu, cpu->memory[addr], cpu->memory[addr + 1], offset)
#define READ_INDEXED_INDIRECT_PTR_OFFSET(cpu, offset)                                                                                                                                                                                                          \
	arg1 = READ_OCTET(cpu);                                                                                                                                                                                                                                    \
	r = GET_INDEXED_INDIRECT(cpu, arg1, offset)

#define READ_INDEXED_INDIRECT_ZP_OFFSET(cpu, zp_offset)                                                                                                                                                                                                        \
	arg1 = ZP(READ_OCTET(cpu) + zp_offset);                                                                                                                                                                                                                    \
	r = GET_INDEXED_INDIRECT(cpu, arg1, 0)

#define ZP(v) ((uint8_t) v)
#define GET_ZP_MEM(cpu) cpu->memory[READ_OCTET(cpu)]
#define GET_ZP_MEM_OFFSET(cpu, offset) (cpu->memory[ZP(READ_OCTET(cpu) + offset)])
#define STACK_START 0x0100
#define STACK_PUSH(cpu, v) (cpu)->memory[(cpu)->sp-- + STACK_START] = v
#define STACK_POP(cpu) (cpu)->memory[++(cpu)->sp + STACK_START]

#define READ_IMMEDIATE READ_OCTET

#define READ_ABSOLUTE_MEMORY_ADDRESS(cpu, offset)                                                                                                                                                                                                              \
	arg1 = READ_OCTET(cpu);                                                                                                                                                                                                                                    \
	arg2 = READ_OCTET(cpu);                                                                                                                                                                                                                                    \
	r1 = ABSOLUTE_MEMORY_OFFSET(arg1, arg2, offset)

#define READ_ABSOLUTE_MEMORY_WITH_OFFSET(cpu, offset)                                                                                                                                                                                                          \
	arg1 = READ_OCTET(cpu);                                                                                                                                                                                                                                    \
	arg2 = READ_OCTET(cpu);                                                                                                                                                                                                                                    \
	r = GET_FROM_MEMORY_OFFSET(cpu, arg1, arg2, offset);

#define READ_INDEXED_INDIRECT_ADDRESS(cpu, addr, offset) ABSOLUTE_MEMORY_OFFSET(cpu->memory[addr + offset], cpu->memory[addr + offset + 1], 0)

#define READ_INDEXED_INDIRECT_MEMORY_ADDRESS_ZP_OFFSET(cpu, off)                                                                                                                                                                                               \
	arg1 = ZP(READ_OCTET(cpu) + off);                                                                                                                                                                                                                          \
	r1 = READ_INDEXED_INDIRECT_ADDRESS(cpu, arg1, 0)

#define READ_INDEXED_INDIRECT_MEMORY_ADDRESS_PTR_OFFSET(cpu, off)                                                                                                                                                                                              \
	arg1 = READ_OCTET(cpu);                                                                                                                                                                                                                                    \
	r1 = READ_INDEXED_INDIRECT_ADDRESS(cpu, arg1, 0) + off

#define READ_BRANCH_WITH_FLAG(cpu, flag, on)                                                                                                                                                                                                                   \
	arg1 = READ_OCTET(cpu);                                                                                                                                                                                                                                    \
	if (get_flag(cpu, flag) == on) {                                                                                                                                                                                                                           \
		cpu->pc += (int8_t) arg1;                                                                                                                                                                                                                              \
	}

static inline void cmp(zany_cpu* cpu, uint8_t mem, uint8_t reg)
{
	set_flag(cpu, FLAG_CARRY, reg >= mem);
	set_flag(cpu, FLAG_NEGATIVE, 0x80 & (reg - mem));
	set_flag(cpu, FLAG_ZERO, reg == mem);
}

static inline void add(zany_cpu* cpu, uint8_t r1)
{
	uint16_t extended = r1 + cpu->a + get_flag(cpu, FLAG_CARRY);
	set_flag(cpu, FLAG_CARRY, extended & 0xFF00);
	set_flag(cpu, FLAG_NEGATIVE, ((uint8_t) extended) & 0x80);
	set_flag(cpu, FLAG_OVERFLOW, (cpu->a & 0x80) != (extended & 0x80));
	set_flag(cpu, FLAG_ZERO, ((uint8_t) extended) == 0);
	cpu->a = (uint8_t) extended;
}

static inline void sub(zany_cpu* cpu, uint8_t r1)
{
	uint16_t extended = r1 + cpu->a - r1 - !get_flag(cpu, FLAG_CARRY);
	set_flag(cpu, FLAG_CARRY, !(extended & 0x8000));
	set_flag(cpu, FLAG_NEGATIVE, extended & 0x80);
	set_flag(cpu, FLAG_OVERFLOW, 0xFF00 & extended);
	set_flag(cpu, FLAG_ZERO, extended == 0);
	cpu->a = (uint8_t) extended;
}

#if defined ZANY_DEBUG_OPCODE
static void sleep_a_while()
{
	struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = 100000000;

	nanosleep(&ts, &ts);
}
#endif

int zany_run(zany_cpu* cpu)
{
	uint8_t r, arg1, arg2;
	uint16_t r1, r2;

	for (;;) {
		uint8_t opcode = READ_OCTET(cpu);
#if defined ZANY_DEBUG_OPCODE
		TYRAN_LOG_INFO("opcode:%02X a:%02X x:%02X y:%02X pc:%04X sr:%02X", opcode, cpu->a, cpu->x, cpu->y, (cpu->pc - 1), cpu->sr);
#endif
		switch (opcode) {
#include "opcodes/arithmetic.inc"
#include "opcodes/branch.inc"
#include "opcodes/compare.inc"
#include "opcodes/flags.inc"
#include "opcodes/incdec.inc"
#include "opcodes/jump.inc"
#include "opcodes/load.inc"
#include "opcodes/logical.inc"
#include "opcodes/rotate.inc"
#include "opcodes/stack.inc"
#include "opcodes/store.inc"
#include "opcodes/transfer.inc"
			case NOP:
				break;
			default:
				TYRAN_LOG_WARN("Unknown opcode:%02X", opcode);
				return -1;
				break;
		}
#if defined ZANY_DEBUG_OPCODE
		sleep_a_while();
#endif
	}
	return 0;
}
