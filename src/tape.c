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

#include "tape.h"

/**
 * @brief Read all blocks from a tape
 * 
 * @return uint8_t number of blocks read
 */
uint8_t read_tape(void) {
    sprintf(&vidmem[0x50*4], "Press any key to read the tape.");
    while(keymem[0x0C] == 0) {} // wait until a key is pressed
    
    // rewind the tape
    clearline(4);
    sprintf(&vidmem[0x50*4], "Rewinding tape.");
    tape_rewind();
    sprintf(&vidmem[0x50*4], "Reading data from tape...");

    // clear program block lines
    clearlines(PROGRESSLINE, PROGRESSLINE+4);

    // rewind the tape
    tape_rewind();

    // which block from tape is being read
    uint8_t blockctr = 0;

    while(memory[CASSTAT] != 'M') { // keep reading while no error is encountered

        // read block from the tape
        tape_read_block();

        // show current program
        sprintf(&vidmem[0x50*OPERATIONLINE], "Storing program in memory:");
        show_current_program(PROGRAMLINE);

        // store header data in upper memory
        memcpy(&memory[MEMHEADER + blockctr * 0x20], &memory[TAPEHEAD], 0x20);

        // store tape data in bankable memory
        uint8_t bank = blockctr / 8;
        uint8_t blockid = blockctr % 8;
        set_bank(bank);
        memcpy(&memory[MEMDATA + blockid * 0x400], &memory[BUFFER], 0x400);
        
        // print block counter
        sprintf(&vidmem[0x50*PROGRESSLINE], "Blocks in memory");
        uint8_t line = PROGRESSLINE + 1 + blockctr / 12;
        uint8_t item = blockctr % 12;
        vidmem[0x50 * line + item*3 + 1] = COL_WHITE;
        vidmem[0x50 * line + item*3 + 2] = 127;
        vidmem[0x50 * line + item*3 + 3] = 127;

        // increment block counter
        blockctr++;

        if(memory[CASSTAT] != 0) {
            if(memory[CASSTAT] == 'C') {
                sprintf(&vidmem[0x50*20], "Checksum error encountered; block %02i.", blockctr);
            } else {
                sprintf(&vidmem[0x50*20], "Stop reading tape, exit code: %c", memory[CASSTAT]);
                break;
            }
        }
    }

    // clear program lines
    clearlines(OPERATIONLINE, PROGRAMLINE);

    return blockctr;
}

/**
 * @brief Write programs from memory to tape
 * 
 * @param totalblocks total number of blocks to write
 */
void write_tape(uint8_t totalblocks) {
    // provide message to the user to change the tape
    sprintf(&vidmem[0x50*4], " Insert tape to write programs to.");

    // and warn that any existing data on that tape will be overwritten
    vidmem[0x50*6] = COL_RED;
    sprintf(&vidmem[0x50*6+1], "WARNING: This will overwrite any data");
    vidmem[0x50*7] = COL_RED;
    sprintf(&vidmem[0x50*7+1], "currently on the tape.");

    sprintf(&vidmem[0x50*9], " PRESS ANY KEY TO CONTINUE");

    // set all blocks to white
    markblocks(0, totalblocks, COL_WHITE);

    // wait for any key being pressed
    wait_for_key();

    // clear lines
    clearlines(4,8);

    // rewind the tape
    sprintf(&vidmem[0x50*4], "Rewinding tape.");
    tape_rewind();
    sprintf(&vidmem[0x50*4], "Writing %02i block(s) to tape", totalblocks);

    /**
     * When writing back to the tape, we first have to reposition all the blocks
     * in the right position in conventional memory in a sequential fashion
     * and update the tape header. The tape write instruction is only
     * executed once per "file".
     */

    // store number of blocks to write back
    uint8_t blockctr = 0;

    while(blockctr < totalblocks) {

        // retrieve header data from upper memory
        memcpy(&memory[TAPEHEAD], &memory[MEMHEADER + blockctr * 0x20], 0x20);

        // store number of blocks
        uint8_t nrblocksfile = memory[BLOCKCTR];
        markblocks(blockctr, blockctr+nrblocksfile, COL_CYAN);

        // print file information
        sprintf(&vidmem[0x50*OPERATIONLINE], "Storing program on tape:");
        show_current_program(PROGRAMLINE);

        // retrieve all blocks and place them in their canonical memory position
        for(uint8_t i=0; i<nrblocksfile; i++) {
            uint16_t target = (memory[TRANSFER] | (memory[TRANSFER+1] << 8)) + i * 0x400;
            uint8_t bank = (blockctr+i) / 8;
            uint8_t blockid = (blockctr+i) % 8;
            set_bank(bank);
            memcpy(&memory[target], &memory[MEMDATA + blockid * 0x400], 0x400);
        }

        // write data back to tape
        markblocks(blockctr, blockctr+nrblocksfile, COL_CYAN);
        tape_write_block();
        markblocks(blockctr, blockctr+nrblocksfile, COL_GREEN);

        // increment blockcounter
        blockctr += nrblocksfile;
    }

    // clear program lines
    clearlines(OPERATIONLINE, PROGRAMLINE);

    // write end of tape marker
    tape_write_eot();
}

/**
 * @brief Verify all blocks on tape with the contents in memory
 * 
 * @param totalblocks number of blocks to compare
 */
void check_tape(uint8_t totalblocks) {
    // set all blocks to white
    markblocks(0, totalblocks, COL_WHITE);

    // rewind the tape
    sprintf(&vidmem[0x50*4], "Rewinding tape.");
    tape_rewind();
    sprintf(&vidmem[0x50*4], "Checking %02i block(s) on tape", totalblocks);

    for(uint8_t i=0; i<totalblocks; i++) {
        // read block from the tape
        tape_read_block();

        // show current program
        sprintf(&vidmem[0x50*OPERATIONLINE], "Checking program in memory:");
        show_program_from_memory(PROGRAMLINE, i);

        // determine position in memory
        uint8_t bank = i/8;
        uint8_t blockid = i%8;

        // check if tape contents is correct
        uint8_t flag = 0;
        set_bank(bank);
        for(uint16_t j=0; j<0x400; j++) {
            if(memory[MEMDATA + blockid * 0x400 + j] != memory[BUFFER+j]) {
                flag += 1;
                break;
            }
        }

        // check if tape header is correct
        for(uint8_t j=0; j<0x20; j++) {
            if(memory[MEMHEADER + i * 0x20 + j] != memory[TAPEHEAD + j]) {
                flag += 2;
                break;
            }
        }

        switch(flag) {
            case 0:
                markblocks(i, i+1, COL_GREEN);
            break;
            case 1:
                markblocks(i, i+1, COL_RED);
            break;
            case 2:
                markblocks(i, i+1, COL_CYAN);
            break;
            case 3:
                markblocks(i, i+1, COL_RED);
            break;
        }
    }

    // clear program lines
    clearlines(OPERATIONLINE, PROGRAMLINE);
}

/**
 * @brief Show information about the current program on the screen, data is
 *        grabbed from the current cassette header data in memory.
 * 
 * @param line which line to display the information at (zero-based)
 */
void show_current_program(uint8_t line) {
    vidmem[0x50 * line] = COL_YELLOW;

    // program name
    char progname[17];
    progname[16] = '\0';
    memcpy(&progname[0], &memory[DESC1], 8);
    memcpy(&progname[8], &memory[DESC2], 8);
    sprintf(&vidmem[0x50*line+1], "%s", progname);

    // extension
    char ext[4];
    ext[3] = '\0';
    memcpy(&ext[0], &memory[EXT], 3);
    sprintf(&vidmem[0x50*line+18], "%s", ext);

    // type
    vidmem[0x50*line+22] = COL_CYAN;
    vidmem[0x50*line+23] = memory[FILETYPE];
    
    // number of blocks and number of bytes
    uint8_t nrblocks = memory[BLOCKCTR];
    uint16_t nrbytes = memory[FILESIZE] | (memory[FILESIZE+1] << 8);
    sprintf(&vidmem[0x50*line+25], "%2i %5i", nrblocks, nrbytes);
}

/**
 * @brief Show information about a program already residing in memory.
 * 
 * @param line which line to display the information at (zero-based)
 * @param program_id which program to display the information for (zero-based)
 */
void show_program_from_memory(uint8_t line, uint8_t program_id) {
    uint16_t mempos = MEMHEADER + program_id * 0x20;
    vidmem[0x50 * line] = COL_YELLOW;

    // program name
    char progname[17];
    progname[16] = '\0';
    memcpy(&progname[0], &memory[mempos + 0x06], 8);
    memcpy(&progname[8], &memory[mempos + 0x17], 8);
    sprintf(&vidmem[0x50*line+1], "%s", progname);

    // extension
    char ext[4];
    ext[3] = '\0';
    memcpy(&ext[0], &memory[mempos + 0x0E], 3);
    sprintf(&vidmem[0x50*line+18], "%s", ext);

    // type
    vidmem[0x50*line+22] = COL_CYAN;
    vidmem[0x50*line+23] = memory[mempos + 0x11];
    
    // number of blocks and number of bytes
    uint8_t nrblocks = memory[mempos + 0x1F];
    uint16_t nrbytes = memory[mempos + 0x04] | (memory[mempos + 0x05] << 8);
    sprintf(&vidmem[0x50*line+25], "%2i %5i", nrblocks, nrbytes);
}

/**
 * @brief Mark blocks on the display with a color
 * 
 * @param start first block
 * @param stop last block
 * @param color color
 */
void markblocks(uint8_t start, uint8_t stop, uint8_t color) {
    for(uint8_t i=0; i<4; i++) {
        for(uint8_t j=0; j<12; j++) {

            if(i*12+j >= start && i*12+j < stop) {
                vidmem[0x50*(i+PROGRESSLINE+1)+j*3+1] = color;   
            }
        }
    }
}

/**
 * @brief Print the cassette header as hex-characters on the screen
 * 
 * @param line which line to start printing at (zero-based)
 */
void print_header_hex(uint8_t line) {
    for(uint8_t i=0; i<4; i++) {
        vidmem[0x50 * (i+line)] = COL_CYAN;
        for(uint8_t j=0; j<8; j++) {
            printhex(0x50 * (i+line) + j*3 + 2, memory[TAPEHEAD + i*8+j]);
        }
    }
}