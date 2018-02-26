case JMP_ABS:
	READ_ABSOLUTE_MEMORY_ADDRESS(cpu, 0);
	cpu->pc = r1;
	break;

case JMP_IND:
	READ_ABSOLUTE_MEMORY_ADDRESS(cpu, 0);
	cpu->pc = ABSOLUTE_MEMORY_OFFSET(cpu->memory[r1], cpu->memory[r1+1], 0);
	break;

case JSR_ABS:
	READ_ABSOLUTE_MEMORY_ADDRESS(cpu, 0);
	if(r1 == 0x1) {
		printf("Special function 0x1. X = %2x\n", cpu->x);
	} else if(r1 == 0x2) {
		printf("Special function 0x2. X = %2x, Y = %2x\n", cpu->x, cpu->y);
	} else if(r1 == 0x3) {
		printf("Special function 0x3. cpu->memory[%04x] = %04x\n", cpu->x, cpu->memory[cpu->x]);
	} else {
		r2 = cpu->pc;
		STACK_PUSH(cpu, (r2 & 0xFF00) >> 8);
		STACK_PUSH(cpu, r2 & 0xFF);
		cpu->pc = r1;
	}
	break;

case RTS:
	if (cpu->sp == 0xff) {
		//TYRAN_LOG_INFO("Run is complete.\n");

		return;
	}
	arg1 = STACK_POP(cpu);
	cpu->pc = ABSOLUTE_MEMORY_OFFSET(arg1, STACK_POP(cpu), 0);
	break;
