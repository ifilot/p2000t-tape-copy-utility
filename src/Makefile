main.bin main.map main.rom: main.c memory.c util.c tape.asm tape.c
	zcc \
	+embedded -clib=sdcc_iy \
	main.c \
	memory.c \
	util.c \
	tape.asm \
	tape.c \
	-startup=1 \
	-pragma-define:CRT_ORG_CODE=0x1000 \
	-pragma-define:CRT_ORG_DATA=0xD400 \
	-pragma-define:REGISTER_SP=0xDFFF \
	-pragma-define:CRT_STACK_SIZE=256 \
	-pragma-define:CRT_INCLUDE_PREAMBLE=1 \
	-pragma-define:CLIB_FOPEN_MAX=0 \
	--max-allocs-per-node2000 \
	-SO3 -bn main.bin \
	-create-app -m
