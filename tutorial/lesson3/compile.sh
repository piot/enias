#!/bin/bash

ca65 lesson3.s
ld65 -C ../../asm/enias.cfg lesson3.o
../../src/enias a.out
