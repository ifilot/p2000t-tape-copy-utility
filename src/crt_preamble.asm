; signature, byte count, checksum
DB 0x5E,0x00,0x00,0x00,0x00

; name of the cartridge (11 bytes)
DB "TAPE-CPTOOL"

jp __Start