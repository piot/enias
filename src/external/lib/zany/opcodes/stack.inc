case PHA:
    STACK_PUSH(cpu, cpu->a);
    break;

case PLA:
    cpu->a = STACK_POP(cpu);
	set_flag(cpu, FLAG_ZERO, !cpu->a);
	set_flag(cpu, FLAG_NEGATIVE, cpu->a & 0x80);
    break;
