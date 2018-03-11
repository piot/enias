/* --------------------------------------- */
case ASL_ABS:
    READ_ABSOLUTE_MEMORY_ADDRESS(cpu, 0);
    set_flag(cpu, FLAG_CARRY, cpu->memory[r1] & 0x80);
    cpu->memory[r1] = (cpu->memory[r1] << 1) & 0xFE;
    set_flags(cpu, cpu->memory[r1]);
    break;

case ASL_ABS_X:
    READ_ABSOLUTE_MEMORY_ADDRESS(cpu, cpu->x);
    set_flag(cpu, FLAG_CARRY, cpu->memory[r1] & 0x80);
    cpu->memory[r1] = (cpu->memory[r1] << 1) & 0xFE;
    set_flags(cpu, cpu->memory[r1]);
    break;

case ASL_ACC:
    set_flag(cpu, FLAG_CARRY, cpu->a & 0x80);
    cpu->a = (cpu->a << 1) & 0xFE;
    set_flags(cpu, cpu->a);
    break;

case ASL_ZP:
    arg1 = READ_IMMEDIATE(cpu);
    set_flag(cpu, FLAG_CARRY, cpu->memory[arg1] & 0x80);
    cpu->memory[arg1] = (cpu->memory[arg1] << 1) & 0xFE;
    set_flags(cpu, cpu->memory[arg1]);
    break;

case ASL_ZP_X:
    arg1 = FORCE_TO_ZP_ADDRESS(READ_IMMEDIATE(cpu) + cpu->x);
    set_flag(cpu, FLAG_CARRY, cpu->memory[arg1] & 0x80);
    cpu->memory[arg1] = (cpu->memory[arg1] << 1) & 0xFE;
    set_flags(cpu, cpu->memory[arg1]);
    break;

/* --------------------------------------- */

case LSR_ABS:
    READ_ABSOLUTE_MEMORY_ADDRESS(cpu, 0);
    set_flag(cpu, FLAG_CARRY, cpu->memory[r1] & 0x01);
    cpu->memory[r1] = (cpu->memory[r1] >> 1) & 0x7F;
    set_flags(cpu, cpu->memory[r1]);
    break;

case LSR_ABS_X:
	READ_ABSOLUTE_MEMORY_ADDRESS(cpu, cpu->x);
    set_flag(cpu, FLAG_CARRY, cpu->memory[r1] & 0x01);
    cpu->memory[r1] = (cpu->memory[r1] >> 1) & 0x7F;
    set_flags(cpu, cpu->memory[r1]);
    break;

case LSR_ACC:
    set_flag(cpu, FLAG_CARRY, cpu->a & 0x01);
    cpu->a = (cpu->a >> 1) & 0x7F;
    set_flags(cpu, cpu->a);
    break;

case LSR_ZP:
    arg1 = READ_IMMEDIATE(cpu);
    set_flag(cpu, FLAG_CARRY, cpu->memory[arg1] & 0x01);
    cpu->memory[arg1] = (cpu->memory[arg1] >> 1) & 0x7F;
    set_flags(cpu, cpu->memory[arg1]);
    break;

case LSR_ZP_X:
    arg1 = FORCE_TO_ZP_ADDRESS(READ_IMMEDIATE(cpu) + cpu->x);
    set_flag(cpu, FLAG_CARRY, cpu->memory[arg1] & 0x01);
    cpu->memory[arg1] = (cpu->memory[arg1] >> 1) & 0x7F;
    set_flags(cpu, cpu->memory[arg1]);
    break;

/* --------------------------------------- */

case ROL_ABS:
	READ_ABSOLUTE_MEMORY_ADDRESS(cpu, 0);
    r = cpu->memory[r1] & 0x80;
    cpu->memory[r1] = ((cpu->memory[r1] << 1) & 0xFE) | get_flag(cpu, FLAG_CARRY);
    set_flag(cpu, FLAG_CARRY, r);
    set_flags(cpu, cpu->memory[r1]);
    break;

case ROL_ABS_X:
	READ_ABSOLUTE_MEMORY_ADDRESS(cpu, cpu->x);
    r = cpu->memory[r1] & 0x80;
    cpu->memory[r1] = ((cpu->memory[r1] << 1) & 0xFE) | get_flag(cpu, FLAG_CARRY);
    set_flag(cpu, FLAG_CARRY, r);
    set_flags(cpu, cpu->memory[r1]);
    break;

case ROL_ACC:
    r = cpu->a & 0x80;
    cpu->a = ((cpu->a << 1) & 0xFE) | get_flag(cpu, FLAG_CARRY);
    set_flag(cpu, FLAG_CARRY, r);
    set_flags(cpu, cpu->a);
    break;

case ROL_ZP:
    arg1 = READ_IMMEDIATE(cpu);
    r = cpu->memory[arg1] & 0x80;
    cpu->memory[arg1] = ((cpu->memory[arg1] << 1) & 0xFE) | get_flag(cpu, FLAG_CARRY);
    set_flag(cpu, FLAG_CARRY, r);
    set_flags(cpu, cpu->memory[arg1]);
    break;

case ROL_ZP_X:
    arg1 = FORCE_TO_ZP_ADDRESS(READ_IMMEDIATE(cpu) + cpu->x);
    r = cpu->memory[arg1] & 0x80;
    cpu->memory[arg1] = ((cpu->memory[arg1] << 1) & 0xFE) | get_flag(cpu, FLAG_CARRY);
    set_flag(cpu, FLAG_CARRY, r);
    set_flags(cpu, cpu->memory[arg1]);
    break;

/* --------------------------------------- */

case ROR_ABS:
	READ_ABSOLUTE_MEMORY_ADDRESS(cpu, 0);
    r = cpu->memory[r1] & 0x01;
    cpu->memory[r1] = ((cpu->memory[r1] >> 1) & 0x7F) | (get_flag(cpu, FLAG_CARRY) << 7);
    set_flag(cpu, FLAG_CARRY, r);
    set_flags(cpu, cpu->memory[r1]);
    break;

case ROR_ABS_X:
	READ_ABSOLUTE_MEMORY_ADDRESS(cpu, cpu->x);
    r = cpu->memory[r1] & 0x01;
    cpu->memory[r1] = ((cpu->memory[r1] >> 1) & 0x7F) | (get_flag(cpu, FLAG_CARRY) << 7);
    set_flag(cpu, FLAG_CARRY, r);
    set_flags(cpu, cpu->memory[r1]);
    break;

case ROR_ACC:
    r = cpu->a & 0x01;
    cpu->a = ((cpu->a >> 1) & 0x7F) | (get_flag(cpu, FLAG_CARRY) << 7);
    set_flag(cpu, FLAG_CARRY, r);
    set_flags(cpu, cpu->a);
    break;

case ROR_ZP:
    arg1 = READ_IMMEDIATE(cpu);
    r = cpu->memory[arg1] & 0x01;
    cpu->memory[arg1] = ((cpu->memory[arg1] >> 1) & 0x7F) | (get_flag(cpu, FLAG_CARRY) << 7);
    set_flag(cpu, FLAG_CARRY, r);
    set_flags(cpu, cpu->memory[arg1]);
    break;

case ROR_ZP_X:
    arg1 = FORCE_TO_ZP_ADDRESS(READ_IMMEDIATE(cpu) + cpu->x);
    r = cpu->memory[arg1] & 0x01;
    cpu->memory[arg1] = ((cpu->memory[arg1] >> 1) & 0x7F) | (get_flag(cpu, FLAG_CARRY) << 7);
    set_flag(cpu, FLAG_CARRY, r);
    set_flags(cpu, cpu->memory[arg1]);
    break;

/* --------------------------------------- */
