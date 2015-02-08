TARGET = main

SRC = $(wildcard *.c)

MSPDEBUG = mspdebug
MSPPROGRAMMER = rf2500

CC = msp430-gcc

# Compiler flag to set the C Standard level.
#     gnu89 = c89 plus GCC extensions
#     gnu99 = c99 plus GCC extensions
CSTANDARD = -std=gnu99 -mmcu=msp430g2553

all: build
	
install: build
	$(MSPDEBUG) $(MSPPROGRAMMER) "erase"
	$(MSPDEBUG) $(MSPPROGRAMMER) "prog $(TARGET).elf"

build:
	$(CC) $(CSTANDARD) -oS -o $(TARGET).elf main.c

debug:
	$(CC) $(CSTANDARD) -oS -g -o $(TARGET).elf $(SRC)

mspdebug:
	$(MSPDEBUG) $(MSPPROGRAMMER)

pwm:
	$(CC) $(CSTANDARD) -oS -o pwm.elf pwm.c
	$(MSPDEBUG) $(MSPPROGRAMMER) "erase"
	$(MSPDEBUG) $(MSPPROGRAMMER) "prog pwm.elf"



clean:
	rm $(TARGET).elf
