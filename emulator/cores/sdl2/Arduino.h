#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#ifndef WEAK
#define WEAK __attribute__ ((weak))
#endif

extern void setup(void) ;
extern void loop(void) ;
