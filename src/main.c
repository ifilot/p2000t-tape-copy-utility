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

int main(void) {
    init();

    sprintf(&vidmem[0x50*3], "Press any key to read the tape.");
    while(keymem[0x0C] == 0) {} // wait until a key is pressed
    clearline(3);

    // rewind the tape
    tape_rewind();

    uint8_t line = 4;
    uint8_t iter = 0;
    uint8_t blockctr = 0; // which block from tape is being read

    while(memory[CASSTAT] != 'M') { // keep reading while no error is encountered

        // read block from the tape
        tape_read_block();

        // print current tape header to screen
        sprintf(&vidmem[0x50*4], "Block header (%02i)", blockctr+1);
        for(uint8_t i=0; i<4; i++) {
            vidmem[0x50 * (i+5)] = COL_CYAN;
            for(uint8_t j=0; j<8; j++) {
                printhex(0x50 * (i+5) + j*3 + 2, memory[TAPEHEAD + i*8+j]);
            }
        }

        // store header data in upper memory
        memcpy(&memory[0xDD00 + blockctr * 0x20], &memory[TAPEHEAD], 0x20);

        // store tape data in bankable memory
        uint8_t bank = blockctr / 8;
        uint8_t blockid = blockctr % 8;
        set_bank(bank);
        memcpy(&memory[0xE000 + blockid * 0x400], &memory[BUFFER], 0x400);
        
        // increment block counter
        blockctr++;

        if(memory[CASSTAT] != 0) {
            sprintf(&vidmem[0x50*20], "Stop reading tape, exit code: %c", memory[CASSTAT]);
            break;
        }
    }

    sprintf(&vidmem[0x50*10], "Insert tape to write to.");
    sprintf(&vidmem[0x50*11], "and press any key.");
    keymem[0x0C] = 0;
    while(keymem[0x0C] == 0) {} // wait until a key is pressed
    clearline(20);

    // rewind the tape
    tape_rewind();

    // store number of blocks to write back
    uint8_t totalblocks = blockctr;
    blockctr = 0;

    /**
     * When writing back to the tape, we first have to reposition all the blocks
     * in the right position in conventional memory in a sequential fashion
     * and update the tape header. The tape write instruction is only
     * executed once per "file".
     */

    while(blockctr < totalblocks) {
        sprintf(&vidmem[0x50*13], "Writing block %02i / %02i...", blockctr+1, totalblocks);

        // retrieve header data from upper memory
        memcpy(&memory[TAPEHEAD], &memory[0xDD00 + blockctr * 0x20], 0x20);

        // print header
        sprintf(&vidmem[0x50*4], "Block header (%02i)", blockctr+1);
        for(uint8_t i=0; i<4; i++) {
            vidmem[0x50 * (i+16)] = COL_GREEN;
            for(uint8_t j=0; j<8; j++) {
                printhex(0x50 * (i+16) + j*3 + 2, memory[TAPEHEAD + i*8+j]);
            }
        }

        // retrieve all blocks and place them in their canonical memory position
        for(uint8_t i=0; i<memory[BLOCKCTR]; i++) {
            uint16_t target = memory[TRANSFER] + i * 0x400;
            uint8_t bank = (blockctr+i) / 8;
            uint8_t blockid = (blockctr+i) % 8;
            set_bank(bank);
            memcpy(&memory[target], &memory[0xE000 + blockid * 0x400], 0x400);
        }

        // write data back to tape
        tape_write_block();
    }

    sprintf(&vidmem[0x50*21], "All done");

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