#!/bin/bash

ca65 lesson1.s
ld65 -C ../../asm/enias.cfg lesson1.o
../../src/enias a.out
