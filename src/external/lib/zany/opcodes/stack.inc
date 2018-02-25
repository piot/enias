case PHA:
    STACK_PUSH(cpu, cpu->a);
    break;

case PLA:
    cpu->a = STACK_POP(cpu);
    break;
