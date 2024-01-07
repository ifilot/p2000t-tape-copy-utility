#include <stdio.h>
#include <z80.h>

#include "memory.h"
#include "tape.h"
#include "util.h"
#include "constants.h"
#include "config.h"

// forward declarations
void init(void);
void set_bank(uint8_t);
void show_current_program(uint8_t);
void markblocks(uint8_t, uint8_t, uint8_t);
void print_header_hex(uint8_t);

int main(void) {
    init();

    sprintf(&vidmem[0x50*3], "Press any key to read the tape.");
    while(keymem[0x0C] == 0) {} // wait until a key is pressed
    clearline(3);

    // rewind the tape
    tape_rewind();

    // which block from tape is being read
    uint8_t blockctr = 0;

    while(memory[CASSTAT] != 'M') { // keep reading while no error is encountered

        // read block from the tape
        tape_read_block();

        // show current program
        sprintf(&vidmem[0x50*4], "Storing program in memory:");
        show_current_program(5);

        // print current tape header to screen
        sprintf(&vidmem[0x50*7], "Block header (%02i)", blockctr+1);
        print_header_hex(8);

        // store header data in upper memory
        memcpy(&memory[MEMHEADER + blockctr * 0x20], &memory[TAPEHEAD], 0x20);

        // store tape data in bankable memory
        uint8_t bank = blockctr / 8;
        uint8_t blockid = blockctr % 8;
        set_bank(bank);
        memcpy(&memory[MEMDATA + blockid * 0x400], &memory[BUFFER], 0x400);
        
        // print block counter
        sprintf(&vidmem[0x50*15], "Progress");
        uint8_t line = 16 + blockctr / 12;
        uint8_t item = blockctr % 12;
        vidmem[0x50 * line + item*3 + 2] = 127;
        vidmem[0x50 * line + item*3 + 3] = 127;

        // increment block counter
        blockctr++;

        if(memory[CASSTAT] != 0) {
            sprintf(&vidmem[0x50*20], "Stop reading tape, exit code: %c", memory[CASSTAT]);
            break;
        }
    }

    // clear block header
    for(uint8_t i=4; i<21; i++) {
        clearline(i);
    }

    // provide message to the user to change the tape
    sprintf(&vidmem[0x50*4], " Insert tape to write programs to");
    sprintf(&vidmem[0x50*5], " and PRESS ANY KEY.");

    // and warn that any existing data on that tape will be overwritten
    vidmem[0x50*7] = COL_RED;
    sprintf(&vidmem[0x50*7+1], "WARNING: This will overwrite any data");
    vidmem[0x50*8] = COL_RED;
    sprintf(&vidmem[0x50*8+1], "currently on the tape.");

    keymem[0x0C] = 0;
    while(keymem[0x0C] == 0) {} // wait until a key is pressed

    // clear lines
    for(uint8_t i=4; i<21; i++) {
        clearline(i);
    }

    // rewind the tape
    sprintf(&vidmem[0x50*4], "Rewinding tape.");
    tape_rewind();
    sprintf(&vidmem[0x50*4], "Writing data to tape...");

    // store number of blocks to write back
    uint8_t totalblocks = blockctr;
    blockctr = 0;

    // show layout of all programs to copy
    for(uint8_t i=0; i<4; i++) {
        for(uint8_t j=0; j<12; j++) {

            if(i*12+j >= totalblocks) {
                break;
            }

            vidmem[0x50*(i+6)+j*3+1] = COL_WHITE;
            vidmem[0x50*(i+6)+j*3+2] = 127;
            vidmem[0x50*(i+6)+j*3+3] = 127;
        }
    }

    /**
     * When writing back to the tape, we first have to reposition all the blocks
     * in the right position in conventional memory in a sequential fashion
     * and update the tape header. The tape write instruction is only
     * executed once per "file".
     */

    while(blockctr < totalblocks) {
        sprintf(&vidmem[0x50*13], "Writing %02i block(s)", totalblocks);

        // retrieve header data from upper memory
        memcpy(&memory[TAPEHEAD], &memory[MEMHEADER + blockctr * 0x20], 0x20);

        // store number of blocks
        uint8_t nrblocksfile = memory[BLOCKCTR];
        markblocks(blockctr, blockctr+nrblocksfile, COL_CYAN);

        // print file information
        sprintf(&vidmem[0x50*12], "Current program:");
        show_current_program(13);

        // retrieve all blocks and place them in their canonical memory position
        for(uint8_t i=0; i<nrblocksfile; i++) {
            uint16_t target = memory[TRANSFER] + i * 0x400;
            uint8_t bank = (blockctr+i) / 8;
            uint8_t blockid = (blockctr+i) % 8;
            set_bank(bank);
            memcpy(&memory[target], &memory[MEMDATA + blockid * 0x400], 0x400);
        }

        // write data back to tape
        markblocks(blockctr, blockctr+nrblocksfile, COL_RED);
        tape_write_block();
        markblocks(blockctr, blockctr+nrblocksfile, COL_GREEN);

        // increment blockcounter
        blockctr += nrblocksfile;
    }

    // write end of tape marker
    tape_write_eot();

    clearline(4);
    sprintf(&vidmem[0x50*15], "All done");

    return 0;
}

void init(void) {
    vidmem[0x50] = TEXT_DOUBLE;
    vidmem[0x50+1] = COL_CYAN;
    sprintf(&vidmem[0x50+2], "TAPE-COPY-UTILITY");
    sprintf(&vidmem[0x50*22], "Version: %s", __VERSION__);
    sprintf(&vidmem[0x50*23], "Compiled at: %s / %s", __DATE__, __TIME__);
}

void set_bank(uint8_t bank) {
    z80_outp(MEMBANK, bank);
}

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

void markblocks(uint8_t start, uint8_t stop, uint8_t color) {
    for(uint8_t i=0; i<4; i++) {
        for(uint8_t j=0; j<12; j++) {

            if(i*12+j >= start && i*12+j < stop) {
                vidmem[0x50*(i+6)+j*3+1] = color;   
            }
        }
    }
}

void print_header_hex(uint8_t line) {
    for(uint8_t i=0; i<4; i++) {
        vidmem[0x50 * (i+line)] = COL_CYAN;
        for(uint8_t j=0; j<8; j++) {
            printhex(0x50 * (i+line) + j*3 + 2, memory[TAPEHEAD + i*8+j]);
        }
    }
}