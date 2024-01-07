# Tape Copy Utility

![GitHub tag (latest SemVer)](https://img.shields.io/github/v/tag/ifilot/p2000t-tape-copy-utility?label=version)
[![ACTION](https://github.com/ifilot/den2obj/actions/workflows/build.yml/badge.svg)](https://github.com/ifilot/den2obj/actions/workflows/build.yml)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

> [!important]
> This cartridge requires a 64kb memory expansion for proper functioning.

## Purpose and features

The Tape Copy Utility creates copies of P2000T tapes. It reads a whole tape
into memory and then writes the content of that tape to one or more other
tapes. After writing a tape, there exist the option to perform a verification
of the copying step.

## Memory lay-out

The utility has been designed to leave the lower memory of the P2000T as much
untouched as possible as this part of the memory is needed to restore programs
back to tapes. Since most BASIC programs start at `0x6547`, this means that the
largest file that can be stored and written back to type is 28,345 bytes. Most
programs do not require that much memory, though a few exceptions exist.

An overview of the memory lay-out can be found in the table below.

Memory address  | Description
--------------- | --------------------------------------------------
`0x0000-0x0FFF` | Monitor routines
`0x1000-0x4FFF` | Cartridge
`0x5000-0x5FFF` | Video memory
`0x6000-0x602F` | Monitor variables (e.g. keyboard)
`0x6030-0x604F` | Cassette variables
`0x604F-0xD3FF` | Free memory to use by the monitor routines
`0xD400-0xD4FF` | Program variables
`0xD500-0xD8FF` | Tape buffer `0x400` bytes
`0xD900-0xDEFF` | Cassette file headers (metadata)
`0xDF00-0xDFFF` | Stack
`0xE000-0xFFFF` | Cassette file data, uses all available banks (0-5)

## Installation

To use this cartridge, download the latest version of 
[TAPECOPY.BIN](https://github.com/ifilot/p2000t-tape-copy-utility/releases/latest/download/TAPECOPY.BIN)
and write the contents of this file to a SLOT1 cartridge. This requires a writeable
cartridge such as the [P2000T ZIF-cartridge](https://www.philips-p2000t.nl/cartridges/zif-cartridge.html).

## Usage

Upon boot, the cartridge offers several options in the menu

* (R) Read tape into memory
* (W) Write tape from memory
* (C) Check tape
* (B) Rewind tape

To copy a tape, first the tape data needs to be placed into memory using the
*Read tape into memory* option. Next, the user can write the tape data to 
another tape via the *Write tape from memory* option. After writing, one 
can verify that the process went correctly using the *Check data* option.