case BCC_REL:
	READ_BRANCH_WITH_FLAG(cpu, FLAG_CARRY, 0);
    break;

case BCS_REL:
	READ_BRANCH_WITH_FLAG(cpu, FLAG_CARRY, 1);
    break;

case BEQ_REL:
	READ_BRANCH_WITH_FLAG(cpu, FLAG_ZERO, 1);
    break;

case BMI_REL:
	READ_BRANCH_WITH_FLAG(cpu, FLAG_NEGATIVE, 1);
    break;

case BNE_REL:
	READ_BRANCH_WITH_FLAG(cpu, FLAG_ZERO, 0);
    break;

case BPL_REL:
	READ_BRANCH_WITH_FLAG(cpu, FLAG_NEGATIVE, 0);
    break;
