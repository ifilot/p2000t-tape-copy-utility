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

#ifndef _UTIL_H
#define _UTIL_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "memory.h"

/**
 * @brief Print hexadecimal character
 * 
 * @param address in video memory
 * @param val character value
 */
void printhex(uint16_t, uint8_t);

/**
 * @brief Clear a line in video memory
 * 
 * @param row line number
 */
void clearline(uint8_t);

/**
 * @brief Clear multiple lines in memory
 * 
 * @param start first line
 * @param stop last line
 */
void clearlines(uint8_t, uint8_t);

/**
 * @brief Clear the whole screen
 * 
 */
void clearscreen(void);

/**
 * @brief Wait for key-press
 * 
 */
void wait_for_key(void);

#endif
