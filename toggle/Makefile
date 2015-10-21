# Xtensa toolchain
PREFIX=~/esp-open-sdk/xtensa-lx106-elf
# Espressif SDK (not needed for standalone toolchain)
#SDK=~/esp-open-sdk/sdk
# esptool
ESPTOOL=~/esptool

# project name
PROJECT=toggle
# source files
SRC=main.c

OBJ=$(SRC:.c=.o)

CC=$(PREFIX)/bin/xtensa-lx106-elf-gcc
CFLAGS=-I. -mlongcalls -Os -pedantic -Wall -Wextra
LDLIBS=-nostdlib -Wl,--start-group -lmain -lnet80211 -lwpa -llwip -lpp -lphy -Wl,--end-group -lgcc
LDFLAGS=-Teagle.app.v6.ld

all: $(PROJECT)-0x00000.bin 

$(PROJECT): $(OBJ)
	$(CC) $(LDFLAGS) $(LDLIBS) -o $(PROJECT) $(OBJ)

$(PROJECT)-0x00000.bin: $(PROJECT)
	$(ESPTOOL)/esptool.py elf2image $^

flash: $(PROJECT)-0x00000.bin
	$(ESPTOOL)/esptool.py write_flash 0 $(PROJECT)-0x00000.bin 0x40000 $(PROJECT)-0x40000.bin

clean:
	rm -f *.o *.bin
	rm -f $(PROJECT)

.PHONY: all flash clean
