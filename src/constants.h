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

#ifndef _CONSTANTS_H
#define _CONSTANTS_H

// variables for screen colors
#define COL_NONE    0x00
#define COL_RED     0x01
#define COL_GREEN   0x02
#define COL_YELLOW  0x03
#define COL_BLUE    0x04
#define COL_MAGENTA 0x05
#define COL_CYAN    0x06
#define COL_WHITE   0x07
#define TEXT_DOUBLE 0x0D

// variables for the cassette header
#define CASSTAT    0x6017
#define TAPEHEAD   0x6030
#define TRANSFER   0x6030
#define LENGTH     0x6032
#define FILESIZE   0x6034
#define DESC1      0x6036
#define DESC2      0x6047
#define EXT        0x603E
#define FILETYPE   0x6041
#define BLOCKCTR   0x604F
#define MEMSIZE    0x605C
#define TAPE       0x0018
#define BUFFER     0xD500

// I/O ports
#define MEMBANK    0x94

// STORAGE LOCATIONS
#define MEMHEADER  0xD900
#define MEMDATA    0xE000

// UI LOCATIONS
#define OPERATIONLINE  11
#define PROGRAMLINE    12
#define PROGRESSLINE   14

#endif // _CONSTANTS_H
