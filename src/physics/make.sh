#!/bin/bash

PROGRAM=$1 # change this to name of a program you want to compile :)
# -O3 for hardware acceleration
clear; clear; gcc -x c -g -fmax-errors=5 -pedantic -Wall -Wextra -Wstrict-prototypes -Wold-style-definition -Wno-unused-parameter -Wno-missing-field-initializers -o $PROGRAM $PROGRAM.c -lm -lSDL2 && ./$PROGRAM
