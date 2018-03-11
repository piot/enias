case CLC:
    set_flag(cpu, FLAG_CARRY, 0);
    break;

case SEC:
    set_flag(cpu, FLAG_CARRY, 1);
    break;

case CLV:
    set_flag(cpu, FLAG_OVERFLOW, 0);
    break;
