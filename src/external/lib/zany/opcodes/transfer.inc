case TAX:
    cpu->x = cpu->a;
    set_flags(cpu, cpu->x);
    break;

case TAY:
    cpu->y = cpu->a;
    set_flags(cpu, cpu->y);
    break;

case TXA:
    cpu->a = cpu->x;
    set_flags(cpu, cpu->a);
    break;

case TYA:
    cpu->a = cpu->y;
    set_flags(cpu, cpu->a);
    break;
