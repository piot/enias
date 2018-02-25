/* --------------------------------------- */
case ADC_ABS:
	READ_ABSOLUTE_MEMORY_WITH_OFFSET(cpu, 0);
    add(cpu, r);
    break;

case ADC_ABS_X:
	READ_ABSOLUTE_MEMORY_WITH_OFFSET(cpu, cpu->x);
	add(cpu, r);
    break;

case ADC_ABS_Y:
	READ_ABSOLUTE_MEMORY_WITH_OFFSET(cpu, cpu->y);
	add(cpu, r);
    break;

case ADC_IMM:
    add(cpu, READ_OCTET(cpu));
    break;

case ADC_IND_X:
    add(cpu, READ_INDEXED_INDIRECT(cpu, cpu->x));
    break;

case ADC_IND_Y:
    add(cpu, READ_INDEXED_INDIRECT(cpu, cpu->y));
    break;

case ADC_ZP:
    add(cpu, GET_ZP_MEM(cpu));
    break;

case ADC_ZPX:
    add(cpu, GET_ZP_MEM_OFFSET(cpu, cpu->x));
    break;

/* --------------------------------------- */

case SBC_ABS:
	READ_ABSOLUTE_MEMORY_WITH_OFFSET(cpu, 0);
    sub(cpu, r);
    break;

case SBC_ABS_X:
	READ_ABSOLUTE_MEMORY_WITH_OFFSET(cpu, cpu->x);
    sub(cpu, r);
    break;

case SBC_ABS_Y:
	READ_ABSOLUTE_MEMORY_WITH_OFFSET(cpu, cpu->y);
	sub(cpu, r);
    break;

case SBC_IMM:
    sub(cpu, READ_OCTET(cpu));
    break;

case SBC_IND_X:
    sub(cpu, READ_INDEXED_INDIRECT(cpu, cpu->x));
    break;

case SBC_IND_Y:
    sub(cpu, READ_INDEXED_INDIRECT(cpu, cpu->y));
    break;

case SBC_ZP:
    sub(cpu, GET_ZP_MEM(cpu));
    break;

case SBC_ZPX:
    sub(cpu, GET_ZP_MEM_OFFSET(cpu, cpu->x));
    break;

/* --------------------------------------- */
