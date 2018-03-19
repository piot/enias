#!/bin/bash

ca65 lesson4.s
ld65 -C ../../asm/enias.cfg lesson4.o
../../src/enias a.out
