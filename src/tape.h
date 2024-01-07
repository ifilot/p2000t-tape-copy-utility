#ifndef _TAPE_H
#define _TAPE_H

#include <stdio.h>

#include "constants.h"
#include "memory.h"
#include "util.h"

// regular routines
uint8_t read_tape(void);
void write_tape(uint8_t);
void check_tape(uint8_t);

// auxiliary routines
void show_current_program(uint8_t);
void show_program_from_memory(uint8_t, uint8_t);
void markblocks(uint8_t, uint8_t, uint8_t);
void print_header_hex(uint8_t);

// assembly routines
void tape_rewind(void);
void tape_read_block(void);
void tape_write_block(void);
void tape_write_eot(void);
void tape_skip_back(void);

#endif // _TAPE_H
