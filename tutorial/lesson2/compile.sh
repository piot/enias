#!/bin/bash

ca65 lesson2.s
ld65 -C ../../asm/enias.cfg lesson2.o
../../src/enias a.out
