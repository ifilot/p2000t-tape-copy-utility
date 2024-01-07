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

#include "util.h"

/**
 * @brief Print hexadecimal character
 * 
 * @param address in video memory
 * @param val character value
 */
void printhex(uint16_t vidaddr, uint8_t val) {
    uint8_t v = (val >> 4) & 0x0F;
    
    if(v < 10) {
        vidmem[vidaddr] = v + 48;
    } else {
        vidmem[vidaddr] = v + 65 - 10;
    }

    vidaddr++;

    v = val & 0x0F;
    if(v < 10) {
        vidmem[vidaddr] = v + 48;
    } else {
        vidmem[vidaddr] = v + 65 - 10;
    }
}

/**
 * @brief Clear a line in video memory
 * 
 * @param row line number
 */
void clearline(uint8_t row) {
    memset(&vidmem[row * 0x50], 0x00, 0x50);
}

/**
 * @brief Clear multiple lines in memory
 * 
 * @param start first line
 * @param stop last line
 */
void clearlines(uint8_t start, uint8_t stop) {
    for(uint8_t i=start; i<=stop; i++) {
        clearline(i);
    }
}

/**
 * @brief Clear the whole screen
 * 
 */
void clearscreen(void) {
    clearlines(0, 24);
}

/**
 * @brief Wait for key-press
 * 
 */
void wait_for_key(void) {
    keymem[0x0C] = 0;
    while(keymem[0x0C] == 0) {} // wait until a key is pressed
}