/* --------------------------------------- */
case INC_ABS:
	READ_ABSOLUTE_MEMORY_ADDRESS(cpu, 0);
    set_flags(cpu, ++cpu->memory[r1]);

    break;

case INC_ABS_X:
    READ_ABSOLUTE_MEMORY_ADDRESS(cpu, cpu->x);
    set_flags(cpu, ++cpu->memory[r1]);

    break;

case INC_ZP:
    r1 = READ_OCTET(cpu);
    set_flags(cpu, ++cpu->memory[r1]);
    break;

case INC_ZP_X:
    r1 = FORCE_TO_ZP_ADDRESS(READ_OCTET(cpu) + cpu->x);
    set_flags(cpu, ++cpu->memory[r1]);

    break;

case INX:
    set_flags(cpu, ++cpu->x);
    break;

case INY:
    set_flags(cpu, ++cpu->y);
    break;

/* --------------------------------------- */

case DEC_ABS:
	READ_ABSOLUTE_MEMORY_ADDRESS(cpu, 0);
    set_flags(cpu, --cpu->memory[r1]);

    break;

case DEC_ABS_X:
	READ_ABSOLUTE_MEMORY_ADDRESS(cpu, cpu->x);
    set_flags(cpu, --cpu->memory[r1]);

    break;

case DEC_ZP:
    r1 = READ_OCTET(cpu);
    set_flags(cpu, --cpu->memory[r1]);

    break;

case DEC_ZP_X:
    r1 = FORCE_TO_ZP_ADDRESS(READ_OCTET(cpu) + cpu->x);
    set_flags(cpu, --cpu->memory[r1]);

    break;

case DEX:
    set_flags(cpu, --cpu->x);
    break;

case DEY:
    set_flags(cpu, --cpu->y);
    break;

/* --------------------------------------- */
