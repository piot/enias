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
    r2 = cpu->pc;
    STACK_PUSH(cpu, (r2 & 0xFF00) >> 8);
    STACK_PUSH(cpu, r2 & 0xFF);
    cpu->pc = r1;
    break;

case RTS:
	if (cpu->sp == 0xff) {
		//TYRAN_LOG_INFO("Run is complete.\n");

		return;
	}
    arg1 = STACK_POP(cpu);
    cpu->pc = ABSOLUTE_MEMORY_OFFSET(arg1, STACK_POP(cpu), 0);
    break;
