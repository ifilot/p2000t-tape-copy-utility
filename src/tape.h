/**************************************************************************
 *                                                                        *
 *   Author: Ivo Filot <ivo@ivofilot.nl>                                  *
 *   https://github.com/ifilot/p2000t-tape-copy-utility
 *                                                                        *
 *   This is free software:                                               *
 *   you can redistribute it and/or modify it under the terms of the      *
 *   GNU General Public License as published by the Free Software         *
 *   Foundation, either version 3 of the License, or (at your option)     *
 *   any later version.                                                   *
 *                                                                        *
 *   This program is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty          *
 *   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.              *
 *   See the GNU General Public License for more details.                 *
 *                                                                        *
 *   You should have received a copy of the GNU General Public License    *
 *   along with this program.  If not, see http://www.gnu.org/licenses/.  *
 *                                                                        *
 **************************************************************************/

#ifndef _TAPE_H
#define _TAPE_H

#include <stdio.h>

#include "constants.h"
#include "memory.h"
#include "util.h"

/*
 * REGULAR ROUTINES
 */ 

/**
 * @brief Read all blocks from a tape
 * 
 * @return uint8_t number of blocks read
 */
uint8_t read_tape(void);

/**
 * @brief Write programs from memory to tape
 * 
 * @param totalblocks total number of blocks to write
 */
void write_tape(uint8_t);

/**
 * @brief Verify all blocks on tape with the contents in memory
 * 
 * @param totalblocks number of blocks to compare
 */
void check_tape(uint8_t);

/*
 * AUXILIARY ROUTINES
 */

/**
 * @brief Show information about the current program on the screen, data is
 *        grabbed from the current cassette header data in memory.
 * 
 * @param line which line to display the information at (zero-based)
 */
void show_current_program(uint8_t);

/**
 * @brief Show information about a program already residing in memory.
 * 
 * @param line which line to display the information at (zero-based)
 * @param program_id which program to display the information for (zero-based)
 */
void show_program_from_memory(uint8_t, uint8_t);

/**
 * @brief Mark blocks on the display with a color
 * 
 * @param start first block
 * @param stop last block
 * @param color color
 */
void markblocks(uint8_t, uint8_t, uint8_t);

/**
 * @brief Print the cassette header as hex-characters on the screen
 * 
 * @param line which line to start printing at (zero-based)
 */
void print_header_hex(uint8_t);

/*
 * ASSEMBLY ROUTINES
 */
void tape_rewind(void);
void tape_read_block(void);
void tape_write_block(void);
void tape_write_eot(void);
void tape_skip_back(void);

#endif // _TAPE_H
