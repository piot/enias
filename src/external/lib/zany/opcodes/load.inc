/* --------------------------------------- */
case LDA_ABS:
	READ_ABSOLUTE_MEMORY_WITH_OFFSET(cpu, 0);
    cpu->a = r;
    set_flags(cpu, cpu->a);
    break;

case LDA_ABS_X:
	READ_ABSOLUTE_MEMORY_WITH_OFFSET(cpu, cpu->x);
	cpu->a = r;
    set_flags(cpu, cpu->a);
    break;

case LDA_ABS_Y:
	READ_ABSOLUTE_MEMORY_WITH_OFFSET(cpu, cpu->y);
	cpu->a = r;
	set_flags(cpu, cpu->a);
    break;

case LDA_IMM:
    cpu->a = READ_OCTET(cpu);
    set_flags(cpu, cpu->a);
    break;

case LDA_IND_X:
    cpu->a = READ_INDEXED_INDIRECT(cpu, cpu->x);
    set_flags(cpu, cpu->a);
    break;

case LDA_IND_Y:
	cpu->a = READ_INDEXED_INDIRECT(cpu, cpu->y);
    set_flags(cpu, cpu->a);
    break;

case LDA_ZP:
    cpu->a = GET_ZP_MEM(cpu);
    set_flags(cpu, cpu->a);
    break;

case LDA_ZP_X:
    cpu->a = GET_ZP_MEM_OFFSET(cpu, cpu->x);
    set_flags(cpu, cpu->a);
    break;

/* --------------------------------------- */

case LDX_ABS:
	READ_ABSOLUTE_MEMORY_WITH_OFFSET(cpu, 0);
    cpu->x = r;
    set_flags(cpu, cpu->x);
    break;

case LDX_ABS_Y:
	READ_ABSOLUTE_MEMORY_WITH_OFFSET(cpu, cpu->y);
    cpu->x = r;
    set_flags(cpu, cpu->x);
    break;

case LDX_IMM:
    cpu->x = READ_OCTET(cpu);
    set_flags(cpu, cpu->x);
    break;

case LDX_ZP:
    cpu->x = GET_ZP_MEM(cpu);
    set_flags(cpu, cpu->x);
    break;

case LDX_ZP_Y:
    cpu->x = GET_ZP_MEM_OFFSET(cpu, cpu->y);
    set_flags(cpu, cpu->x);
    break;

/* --------------------------------------- */

case LDY_ABS:
	READ_ABSOLUTE_MEMORY_WITH_OFFSET(cpu, 0);
    cpu->y = r;
    set_flags(cpu, cpu->y);
    break;

case LDY_ABS_X:
	READ_ABSOLUTE_MEMORY_WITH_OFFSET(cpu, cpu->x);
    cpu->y = r;
    set_flags(cpu, cpu->y);
    break;

case LDY_IMM:
    cpu->y = READ_OCTET(cpu);
    set_flags(cpu, cpu->y);
    break;

case LDY_ZP:
    cpu->y = GET_ZP_MEM(cpu);
    set_flags(cpu, cpu->y);
    break;

case LDY_ZP_X:
    cpu->y = GET_ZP_MEM_OFFSET(cpu, cpu->x);
    set_flags(cpu, cpu->y);
    break;
