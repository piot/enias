/* --------------------------------------- */

case CMP_ABS:
	READ_ABSOLUTE_MEMORY_WITH_OFFSET(cpu, 0);
    cmp(cpu, r, cpu->a);
    break;

case CMP_ABS_X:
	READ_ABSOLUTE_MEMORY_WITH_OFFSET(cpu, cpu->x);
	cmp(cpu, r, cpu->a);
    break;

case CMP_ABS_Y:
	READ_ABSOLUTE_MEMORY_WITH_OFFSET(cpu, cpu->y);
    break;

case CMP_IMM:
	r = READ_OCTET(cpu);
    cmp(cpu, r, cpu->a);
    break;

case CMP_IND_X:
	READ_INDEXED_INDIRECT_ZP_OFFSET(cpu, cpu->x);
    cmp(cpu, r, cpu->a);
    break;

case CMP_IND_Y:
	READ_INDEXED_INDIRECT_PTR_OFFSET(cpu, cpu->y);
    cmp(cpu, r, cpu->a);
    break;

case CMP_ZP:
    cmp(cpu, READ_ZP_MEM(cpu), cpu->a);
    break;

case CMP_ZP_X:
    cmp(cpu, READ_ZP_MEM_OFFSET(cpu, cpu->x), cpu->a);
    break;

/* --------------------------------------- */

case CPX_ABS:
	READ_ABSOLUTE_MEMORY_WITH_OFFSET(cpu, 0);
    cmp(cpu, r, cpu->x);
    break;

case CPX_IMM:
    cmp(cpu, READ_OCTET(cpu), cpu->x);
    break;

case CPX_ZP:
    cmp(cpu, READ_ZP_MEM(cpu), cpu->x);
    break;

/* --------------------------------------- */

case CPY_ABS:
	READ_ABSOLUTE_MEMORY_WITH_OFFSET(cpu, 0);
    cmp(cpu, r, cpu->y);
    break;

case CPY_IMM:
    cmp(cpu, READ_OCTET(cpu), cpu->y);
    break;

case CPY_ZP:
    cmp(cpu, READ_ZP_MEM(cpu), cpu->y);
    break;

/* --------------------------------------- */
