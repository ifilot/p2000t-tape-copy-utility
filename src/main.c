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

#include <stdio.h>
#include <z80.h>

#include "memory.h"
#include "tape.h"
#include "util.h"
#include "constants.h"
#include "config.h"

// forward declarations
void init(void);

int main(void) {
    init();

    // check that the P2000T has at least 48kb or more
    if(memory[0x605C] != 3) {
        sprintf(&vidmem[0x50*4], "It looks like this machine does not have");
        sprintf(&vidmem[0x50*5], "enough memory to run this cartridge.");
        sprintf(&vidmem[0x50*7], ".Terminating.");
        while(1) {}
    }

    uint8_t totalblocks = 0;

    while(1) { // infinite loop over operations
        clearlines(4,9);  // clear lines

        // show list of operations
        sprintf(&vidmem[0x50*4], "Select operation:");
        sprintf(&vidmem[0x50*6], "(R) Read tape into memory");
        sprintf(&vidmem[0x50*7], "(W) Write tape from memory");
        sprintf(&vidmem[0x50*8], "(C) Check tape");
        sprintf(&vidmem[0x50*9], "(B) Rewind tape");
        //sprintf(&vidmem[0x50*10], "(H) Help");

        while(1) { // infinite loop while monitoring key presses
            if(memory[0x600D] == 39) {
                clearlines(4,9);
                totalblocks = read_tape();
                break;
            }

            if(memory[0x600D] == 35) {
                clearlines(4,9);

                if(totalblocks > 0) {
                    write_tape(totalblocks);
                } else {
                    sprintf(&vidmem[0x50*4], "[ERROR] No tape data in memory.");
                    sprintf(&vidmem[0x50*5], "Please perform tape read first.");
                    wait_for_key();
                }
                break;
            }

            if(memory[0x600D] == 28) {
                clearlines(4,9);

                if(totalblocks > 0) {
                    check_tape(totalblocks);
                } else {
                    sprintf(&vidmem[0x50*4], "[ERROR] No tape data in memory.");
                    sprintf(&vidmem[0x50*5], "Please perform tape read first.");
                    wait_for_key();
                }
                break;
            }

            if(memory[0x600D] == 29) {
                clearlines(4,9);
                sprintf(&vidmem[0x50*4], "Rewinding tape.");
                tape_rewind();
                clearline(4);
                break;
            }
        }
    }
}

/**
 * @brief Initialization routine
 */
void init(void) {
    clearscreen();
    vidmem[0x50] = TEXT_DOUBLE;
    vidmem[0x50+1] = COL_CYAN;
    sprintf(&vidmem[0x50+2], "TAPE-COPY-UTILITY");
    sprintf(&vidmem[0x50*22], "Version: %s", __VERSION__);
    sprintf(&vidmem[0x50*23], "Compiled at: %s / %s", __DATE__, __TIME__);
}