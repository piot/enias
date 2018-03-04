/* --------------------------------------- */

case STA_ABS:
	READ_ABSOLUTE_MEMORY_ADDRESS(cpu, 0);
    cpu->memory[r1] = cpu->a;
    break;

case STA_ABS_X:
	READ_ABSOLUTE_MEMORY_ADDRESS(cpu, cpu->x);
    cpu->memory[r1] = cpu->a;
    break;

case STA_ABS_Y:
	READ_ABSOLUTE_MEMORY_ADDRESS(cpu, cpu->y);
    cpu->memory[r1] = cpu->a;
    break;

case STA_IND_X:
	READ_INDEXED_INDIRECT_MEMORY_ADDRESS_ZP_OFFSET(cpu, cpu->x);
    cpu->memory[r1] = cpu->a;
    break;

case STA_IND_Y:
	READ_INDEXED_INDIRECT_MEMORY_ADDRESS_PTR_OFFSET(cpu, cpu->y);
    cpu->memory[r1] = cpu->a;
    break;

case STA_ZP:
    r1 = ZP(READ_IMMEDIATE(cpu));
    cpu->memory[r1] = cpu->a;
    break;

case STA_ZPX:
    r1 = ZP(READ_IMMEDIATE(cpu) + cpu->x);
    cpu->memory[r1] = cpu->a;
    break;

/* --------------------------------------- */

case STX_ZP:
    r1 = ZP(READ_IMMEDIATE(cpu));
    cpu->memory[r1] = cpu->x;
    break;

case STX_ZP_Y:
    r1 = ZP(READ_IMMEDIATE(cpu) + cpu->y);
    cpu->memory[r1] = cpu->x;
    break;

case STX_ABS:
	READ_ABSOLUTE_MEMORY_ADDRESS(cpu, 0);
    cpu->memory[r1] = cpu->x;
    break;

/* --------------------------------------- */

case STY_ZP:
    r1 = ZP(READ_IMMEDIATE(cpu));
    cpu->memory[r1] = cpu->y;
    break;

case STY_ZP_X:
    r1 = ZP(READ_IMMEDIATE(cpu) + cpu->x);
    cpu->memory[r1] = cpu->y;
    break;

case STY_ABS:
	READ_ABSOLUTE_MEMORY_ADDRESS(cpu, 0);
    cpu->memory[r1] = cpu->y;
    break;

/* --------------------------------------- */
