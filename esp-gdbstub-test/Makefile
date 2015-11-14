# always build version with GDB stub (for Travis CI)
DEBUG=y

# Xtensa toolchain
PREFIX=~/esp-open-sdk/xtensa-lx106-elf
# Espressif SDK (not needed for standalone toolchain)
#SDK=~/esp-open-sdk/sdk
# esptool
ESPTOOL=~/esptool
# GDB for Xtensa target
GDB=$(PREFIX)/bin/xtensa-lx106-elf-gdb
# serial port used for esptool and GDB debug stub
SERIAL_PORT=/dev/ttyUSB0

# project name
PROJECT=esp-gdbstub-test
# C source files
SRC=main.c
# Preprocessed assembler source files
SRC_ASM_P=

OBJ=$(SRC:.c=.o)
OBJ_ASM_P=$(SRC_ASM_P:.S=.o)

ifeq ($(DEBUG),y)
	SRC+=../esp-gdbstub/gdbstub.c
	SRC_ASM_P+=../esp-gdbstub/gdbstub-entry.S
endif

CC=$(PREFIX)/bin/xtensa-lx106-elf-gcc

ifeq ($(DEBUG),y)
	CFLAGS=-I. -mlongcalls -Og -ggdb -pedantic -Wall -Wextra -DGDBSTUB_FREERTOS=0 -DDEBUG
else
	CFLAGS=-I. -mlongcalls -Os -pedantic -Wall -Wextra
endif

LDLIBS=-nostdlib -Wl,--start-group -lmain -lnet80211 -lwpa -llwip -lpp -lphy -Wl,--end-group -lgcc
LDFLAGS=-Teagle.app.v6.ld

all: $(PROJECT)-0x00000.bin

%.o: %.S
	$(CC) $(CFLAGS) -c -o $@ $<

$(PROJECT): $(OBJ) $(OBJ_ASM_P)
	$(CC) $(LDFLAGS) $(LDLIBS) -o $(PROJECT) $(OBJ) $(OBJ_ASM_P)

$(PROJECT)-0x00000.bin: $(PROJECT)
	$(ESPTOOL)/esptool.py elf2image $^

flash: $(PROJECT)-0x00000.bin
	$(ESPTOOL)/esptool.py --port $(SERIAL_PORT) write_flash 0 $(PROJECT)-0x00000.bin 0x40000 $(PROJECT)-0x40000.bin

debug:
	$(GDB) -x gdbinit -b 115200 -ex 'target remote $(SERIAL_PORT)' $(PROJECT)

clean:
	rm -f *.o *.bin
	rm -f ../esp-gdbstub/*.o
	rm -f $(PROJECT)

.PHONY: all flash clean debug
