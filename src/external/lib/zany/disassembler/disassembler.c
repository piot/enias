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
#include "../opcode.h"
#include <stdio.h>
#include <zany/disassembler/disassembler.h>

typedef enum zany_argument_type {
	ZANY_DA_IMM,
	ZANY_DA_ZP,
	ZANY_DA_ZP_X,
	ZANY_DA_ZP_Y,
	ZANY_DA_ABS,
	ZANY_DA_ABS_X,
	ZANY_DA_ABS_Y,
	ZANY_DA_IND,
	ZANY_DA_IND_X,
	ZANY_DA_IND_Y,
	ZANY_DA_REL,
	ZANY_DA_NONE,
} zany_argument_type;

typedef struct zany_dis_info {
	uint8_t opcode;
	const char* name;
	zany_argument_type arguments;
} zany_dis_info;

zany_dis_info types[] = {
	{ADC_IMM, "adc", ZANY_DA_IMM},
	{ADC_ZP, "adc", ZANY_DA_ZP},
	{ADC_ZP_X, "adc", ZANY_DA_ZP_X},
	{ADC_ABS, "adc", ZANY_DA_ABS},
	{ADC_ABS_X, "adc", ZANY_DA_ABS_X},
	{ADC_ABS_Y, "adc", ZANY_DA_ABS_Y},
	{ADC_IND_X, "adc", ZANY_DA_IND_X},
	{ADC_IND_Y, "adc", ZANY_DA_IND_Y},

	{AND_IMM, "and", ZANY_DA_IMM},
	{AND_ZP, "and", ZANY_DA_ZP},
	{AND_ZP_X, "and", ZANY_DA_ZP_X},
	{AND_ABS, "and", ZANY_DA_ABS},
	{AND_ABS_X, "and", ZANY_DA_ABS_X},
	{AND_ABS_Y, "and", ZANY_DA_ABS_Y},
	{AND_IND_X, "and", ZANY_DA_IND_X},
	{AND_IND_Y, "and", ZANY_DA_IND_Y},

	{ASL_ACC, "asl", ZANY_DA_NONE},
	{ASL_ZP, "asl", ZANY_DA_ZP},
	{ASL_ZP_X, "asl", ZANY_DA_ZP_X},
	{ASL_ABS, "asl", ZANY_DA_ABS},
	{ASL_ABS_X, "asl", ZANY_DA_ABS_X},

	{BIT_ZP, "bit", ZANY_DA_ZP},
	{BIT_ABS, "bit", ZANY_DA_ABS},

	{BPL_REL, "bpl", ZANY_DA_REL},
	{BMI_REL, "bmi", ZANY_DA_REL},
	{BVC_REL, "bvc", ZANY_DA_REL},
	{BVS_REL, "bvs", ZANY_DA_REL},
	{BCC_REL, "bcc", ZANY_DA_REL},
	{BCS_REL, "bcs", ZANY_DA_REL},
	{BNE_REL, "bne", ZANY_DA_REL},
	{BEQ_REL, "beq", ZANY_DA_REL},

	{CMP_IMM, "cmp", ZANY_DA_IMM},
	{CMP_ZP, "cmp", ZANY_DA_ZP},
	{CMP_ZP_X, "cmp", ZANY_DA_ZP_X},
	{CMP_ABS, "cmp", ZANY_DA_ABS},
	{CMP_ABS_X, "cmp", ZANY_DA_ABS_X},
	{CMP_ABS_X, "cmp", ZANY_DA_ABS_Y},
	{CMP_IND_X, "cmp", ZANY_DA_IND_X},
	{CMP_IND_Y, "cmp", ZANY_DA_IND_Y},

	{CPX_IMM, "cpx", ZANY_DA_IMM},
	{CPX_ZP, "cpx", ZANY_DA_ZP},
	{CPX_ABS, "cpx", ZANY_DA_ABS},

	{CPY_IMM, "cpy", ZANY_DA_IMM},
	{CPY_ZP, "cpy", ZANY_DA_ZP},
	{CPY_ABS, "cpy", ZANY_DA_ABS},

	{DEC_ZP, "dec", ZANY_DA_ZP},
	{DEC_ZP_X, "dec", ZANY_DA_ZP_X},
	{DEC_ABS, "dec", ZANY_DA_ABS},
	{DEC_ABS_X, "dec", ZANY_DA_ABS_X},

	{EOR_IMM, "eor", ZANY_DA_IMM},
	{EOR_ZP, "eor", ZANY_DA_ZP},
	{EOR_ZP_X, "eor", ZANY_DA_ZP_X},
	{EOR_ABS, "eor", ZANY_DA_ABS},
	{EOR_ABS_X, "eor", ZANY_DA_ABS_X},
	{EOR_ABS_X, "eor", ZANY_DA_ABS_Y},
	{EOR_IND_X, "eor", ZANY_DA_IND_X},
	{EOR_IND_Y, "eor", ZANY_DA_IND_Y},

	{CLC, "clc", ZANY_DA_NONE},
	{SEC, "sec", ZANY_DA_NONE},
	{CLV, "clv", ZANY_DA_NONE},
	//	{CLD, "cld", ZANY_DA_NONE},

	{INC_ZP, "inc", ZANY_DA_ZP},
	{INC_ZP_X, "inc", ZANY_DA_ZP_X},
	{INC_ABS, "inc", ZANY_DA_ABS},
	{INC_ABS_X, "inc", ZANY_DA_ABS_X},

	{JMP_ABS, "jmp", ZANY_DA_ABS},
	{JMP_IND, "jmp", ZANY_DA_IND},

	{JSR_ABS, "jsr", ZANY_DA_ABS},

	{LDA_IMM, "lda", ZANY_DA_IMM},
	{LDA_ZP, "lda", ZANY_DA_ZP},
	{LDA_ZP_X, "lda", ZANY_DA_ZP_X},
	{LDA_ABS, "lda", ZANY_DA_ABS},
	{LDA_ABS_X, "lda", ZANY_DA_ABS_X},
	{LDA_ABS_X, "lda", ZANY_DA_ABS_Y},
	{LDA_IND_X, "lda", ZANY_DA_IND_X},
	{LDA_IND_Y, "lda", ZANY_DA_IND_Y},

	{LDX_IMM, "ldx", ZANY_DA_IMM},
	{LDX_ZP, "ldx", ZANY_DA_ZP},
	{LDX_ZP_Y, "ldx", ZANY_DA_ZP_Y},
	{LDX_ABS, "ldx", ZANY_DA_ABS},
	{LDX_ABS_Y, "ldx", ZANY_DA_ABS_Y},

	{LDY_IMM, "ldy", ZANY_DA_IMM},
	{LDY_ZP, "ldy", ZANY_DA_ZP},
	{LDY_ZP_X, "ldy", ZANY_DA_ZP_X},
	{LDY_ABS, "ldy", ZANY_DA_ABS},
	{LDY_ABS_X, "ldy", ZANY_DA_ABS_X},

	{LSR_ACC, "lsr", ZANY_DA_NONE},
	{LSR_ZP, "lsr", ZANY_DA_ZP},
	{LSR_ZP_X, "lsr", ZANY_DA_ZP_X},
	{LSR_ABS, "lsr", ZANY_DA_ABS},
	{LSR_ABS_X, "lsr", ZANY_DA_ABS_X},

	{NOP, "nop", ZANY_DA_NONE},

	{ORA_IMM, "ora", ZANY_DA_IMM},
	{ORA_ZP, "ora", ZANY_DA_ZP},
	{ORA_ZP_X, "ora", ZANY_DA_ZP_X},
	{ORA_ABS, "ora", ZANY_DA_ABS},
	{ORA_ABS_X, "ora", ZANY_DA_ABS_X},
	{ORA_ABS_X, "ora", ZANY_DA_ABS_Y},
	{ORA_IND_X, "ora", ZANY_DA_IND_X},
	{ORA_IND_Y, "ora", ZANY_DA_IND_Y},

	{TAX, "tax", ZANY_DA_NONE},
	{TXA, "txa", ZANY_DA_NONE},
	{DEX, "dex", ZANY_DA_NONE},
	{INX, "inx", ZANY_DA_NONE},
	{TAY, "tay", ZANY_DA_NONE},
	{TYA, "tya", ZANY_DA_NONE},
	{DEY, "dey", ZANY_DA_NONE},
	{INY, "iny", ZANY_DA_NONE},

	{ROL_ACC, "rol", ZANY_DA_NONE},
	{ROL_ZP, "rol", ZANY_DA_ZP},
	{ROL_ZP_X, "rol", ZANY_DA_ZP_X},
	{ROL_ABS, "rol", ZANY_DA_ABS},
	{ROL_ABS_X, "rol", ZANY_DA_ABS_X},

	{ROR_ACC, "ror", ZANY_DA_NONE},
	{ROR_ZP, "ror", ZANY_DA_ZP},
	{ROR_ZP_X, "ror", ZANY_DA_ZP_X},
	{ROR_ABS, "ror", ZANY_DA_ABS},
	{ROR_ABS_X, "ror", ZANY_DA_ABS_X},

	{RTS, "rts", ZANY_DA_NONE},

	{SBC_IMM, "sbc", ZANY_DA_IMM},
	{SBC_ZP, "sbc", ZANY_DA_ZP},
	{SBC_ZP_X, "sbc", ZANY_DA_ZP_X},
	{SBC_ABS, "sbc", ZANY_DA_ABS},
	{SBC_ABS_X, "sbc", ZANY_DA_ABS_X},
	{SBC_ABS_X, "sbc", ZANY_DA_ABS_Y},
	{SBC_IND_X, "sbc", ZANY_DA_IND_X},
	{SBC_IND_Y, "sbc", ZANY_DA_IND_Y},

	{STA_ZP, "sta", ZANY_DA_ZP},
	{STA_ZP_X, "sta", ZANY_DA_ZP_X},
	{STA_ABS, "sta", ZANY_DA_ABS},
	{STA_ABS_X, "sta", ZANY_DA_ABS_X},
	{STA_ABS_X, "sta", ZANY_DA_ABS_Y},
	{STA_IND_X, "sta", ZANY_DA_IND_X},
	{STA_IND_Y, "sta", ZANY_DA_IND_Y},

	{PHA, "pha", ZANY_DA_NONE},
	{PLA, "pla", ZANY_DA_NONE},

	{STX_ZP, "stx", ZANY_DA_ZP},
	{STX_ZP_Y, "stx", ZANY_DA_ZP_Y},
	{STX_ABS, "stx", ZANY_DA_ABS},

	{STY_ZP, "sty", ZANY_DA_ZP},
	{STY_ZP_X, "sty", ZANY_DA_ZP_X},
	{STY_ABS, "sty", ZANY_DA_ABS},

};

int get_argument_size(zany_argument_type arg)
{
	switch (arg) {
		case ZANY_DA_IMM:
			return 1;
		case ZANY_DA_ZP:
		case ZANY_DA_ZP_X:
		case ZANY_DA_ZP_Y:
			return 1;
		case ZANY_DA_ABS:
		case ZANY_DA_ABS_X:
		case ZANY_DA_ABS_Y:
			return 2;
		case ZANY_DA_IND:
		case ZANY_DA_IND_X:
		case ZANY_DA_IND_Y:
			return 1;
		case ZANY_DA_REL:
			return 1;
		case ZANY_DA_NONE:
			return 0;
	}
}

zany_dis_info get_opcode_info(uint8_t opcode)
{
	for (int i = 0; i < sizeof(types) / sizeof(zany_dis_info); ++i) {
		if (types[i].opcode == opcode) {
			return types[i];
		}
	}
	zany_dis_info x = {0, 0, 0};
	return x;
}

const char* get_argument_str(zany_argument_type arg, uint16_t pc, const uint8_t* memory)
{
	static char buf[32];
	uint16_t r2;
	uint16_t r1;
	uint8_t r;
	switch (arg) {
		case ZANY_DA_IMM:
			sprintf(buf, "#$%02x", memory[pc]);
			break;
		case ZANY_DA_ZP:
		case ZANY_DA_ZP_X:
		case ZANY_DA_ZP_Y:
			r = memory[pc];
			sprintf(buf, "<$%02x> [%02X]", r, memory[r]);
			break;
		case ZANY_DA_ABS:
		case ZANY_DA_ABS_X:
		case ZANY_DA_ABS_Y:
			r1 = memory[pc] | (memory[pc + 1] << 8);
			r = memory[r1];
			sprintf(buf, "$%04x [%02X]", r1, r);
			break;
		case ZANY_DA_IND:
		case ZANY_DA_IND_X:
		case ZANY_DA_IND_Y:
			r1 = memory[pc] | (memory[pc + 1] << 8);
			r2 = memory[r1] | (memory[r1 + 1] << 8);
			r = memory[r2];
			sprintf(buf, "($%02x) [%04X %02X]", r1, r2, r);
			break;
		case ZANY_DA_REL:
			sprintf(buf, "+%02d", (char) memory[pc]);
			break;
		case ZANY_DA_NONE:
			buf[0] = 0;
			break;
	}
	return buf;
}

const char* zany_disassembler_string(const uint8_t* memory, uint16_t pc, uint8_t* opcode_size)
{
	static char buf[32];
	uint8_t opcode = memory[pc];
	zany_dis_info info = get_opcode_info(opcode);
	*opcode_size = get_argument_size(info.arguments);

	sprintf(buf, "%s %s", info.name, get_argument_str(info.arguments, pc + 1, memory));
	return buf;
}
