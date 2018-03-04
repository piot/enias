/* --------------------------------------- */

case AND_IMM:
    cpu->a &= READ_IMMEDIATE(cpu);
    set_flags(cpu, cpu->a);
    break;

case AND_ZP:
    cpu->a &= GET_ZP_MEM(cpu);
    set_flags(cpu, cpu->a);
    break;

case AND_ZP_X:
    cpu->a &= GET_ZP_MEM_OFFSET(cpu, cpu->x);
    set_flags(cpu, cpu->a);
    break;

case AND_ABS:
	READ_ABSOLUTE_MEMORY_WITH_OFFSET(cpu, 0)
    cpu->a &= r;
    set_flags(cpu, cpu->a);
    break;

case AND_ABS_X:
	READ_ABSOLUTE_MEMORY_WITH_OFFSET(cpu, cpu->x)
	cpu->a &= r;
	set_flags(cpu, cpu->a);
    break;

case AND_ABS_Y:
	READ_ABSOLUTE_MEMORY_WITH_OFFSET(cpu, cpu->y)
	cpu->a &= r;
	set_flags(cpu, cpu->a);
    break;

case AND_IND_X:
	READ_INDEXED_INDIRECT_ZP_OFFSET(cpu, cpu->x);
    cpu->a &= r;
    set_flags(cpu, cpu->a);
    break;

case AND_IND_Y:
	READ_INDEXED_INDIRECT_PTR_OFFSET(cpu, cpu->y);
    cpu->a &= r;
    set_flags(cpu, cpu->a);
    break;

/* --------------------------------------- */

case EOR_IMM:
    cpu->a ^= READ_IMMEDIATE(cpu);
    set_flags(cpu, cpu->a);
    break;

case EOR_ZP:
    cpu->a ^= GET_ZP_MEM(cpu);
    set_flags(cpu, cpu->a);
    break;

case EOR_ZP_X:
    cpu->a ^= GET_ZP_MEM_OFFSET(cpu, cpu->x);
    set_flags(cpu, cpu->a);
    break;

case EOR_ABS:
	READ_ABSOLUTE_MEMORY_WITH_OFFSET(cpu, 0);
    cpu->a ^= r;
    set_flags(cpu, cpu->a);
    break;

case EOR_ABS_X:
	READ_ABSOLUTE_MEMORY_WITH_OFFSET(cpu, cpu->x);
    cpu->a ^= r;
    set_flags(cpu, cpu->a);
    break;

case EOR_ABS_Y:
	READ_ABSOLUTE_MEMORY_WITH_OFFSET(cpu, cpu->y);
    cpu->a ^= r;
    set_flags(cpu, cpu->a);
    break;

case EOR_IND_X:
	READ_INDEXED_INDIRECT_ZP_OFFSET(cpu, cpu->x);
    cpu->a ^= r;
    set_flags(cpu, cpu->a);
    break;

case EOR_IND_Y:
	READ_INDEXED_INDIRECT_PTR_OFFSET(cpu, cpu->y);
    cpu->a ^= r;
    set_flags(cpu, cpu->a);
    break;

/* --------------------------------------- */

case ORA_IMM:
    cpu->a |= READ_IMMEDIATE(cpu);
    set_flags(cpu, cpu->a);
    break;

case ORA_ZP:
    cpu->a |= GET_ZP_MEM(cpu);
    set_flags(cpu, cpu->a);
    break;

case ORA_ZP_X:
    cpu->a |= GET_ZP_MEM_OFFSET(cpu, cpu->x);
    set_flags(cpu, cpu->a);
    break;

case ORA_ABS:
	READ_ABSOLUTE_MEMORY_WITH_OFFSET(cpu, 0);
    cpu->a |= r;
    set_flags(cpu, cpu->a);
    break;

case ORA_ABS_X:
	READ_ABSOLUTE_MEMORY_WITH_OFFSET(cpu, cpu->x);
    cpu->a |= r;
    set_flags(cpu, cpu->a);
    break;

case ORA_ABS_Y:
	READ_ABSOLUTE_MEMORY_WITH_OFFSET(cpu, cpu->y);
    cpu->a |= r;
    set_flags(cpu, cpu->a);
    break;

case ORA_IND_X:
	READ_INDEXED_INDIRECT_ZP_OFFSET(cpu, cpu->x);
    cpu->a |= r;
    set_flags(cpu, cpu->a);
    break;

case ORA_IND_Y:
	READ_INDEXED_INDIRECT_PTR_OFFSET(cpu, cpu->y);
    cpu->a |= r;
    set_flags(cpu, cpu->a);
    break;

/* --------------------------------------- */

case BIT_ABS:
	READ_ABSOLUTE_MEMORY_WITH_OFFSET(cpu, 0);
    set_flag(cpu, FLAG_ZERO, !(r & cpu->a));
    set_flag(cpu, FLAG_OVERFLOW, r & 0x40);
    set_flag(cpu, FLAG_NEGATIVE, r & 0x80);
    break;

case BIT_ZP:
    arg1 = GET_ZP_MEM(cpu);
    set_flag(cpu, FLAG_ZERO, !(arg1 & cpu->a));
    set_flag(cpu, FLAG_OVERFLOW, arg1 & 0x40);
    set_flag(cpu, FLAG_NEGATIVE, arg1 & 0x80);
    break;

/* --------------------------------------- */
