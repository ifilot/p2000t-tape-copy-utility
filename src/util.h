#ifndef _UTIL_H
#define _UTIL_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "memory.h"

void printhex(uint16_t, uint8_t);
void clearline(uint8_t);
void clearlines(uint8_t, uint8_t);
void clearscreen(void);
void wait_for_key(void);

#endif
