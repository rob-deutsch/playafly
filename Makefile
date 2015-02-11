TARGET = main

SRC = main.c

MSPDEBUG = mspdebug
MSPPROGRAMMER = rf2500

CC = msp430-gcc

# Compiler flag to set the C Standard level.
#     gnu89 = c89 plus GCC extensions
#     gnu99 = c99 plus GCC extensions
CSTANDARD = -std=gnu99 -mmcu=msp430g2553

#TIME = `date +'%s' | tr -d "\n"`
TIME = `date +'%s'`

all: build
	
install: build
	$(MSPDEBUG) $(MSPPROGRAMMER) "erase"
	$(MSPDEBUG) $(MSPPROGRAMMER) "prog $(TARGET).elf"

build:
	$(CC) $(CSTANDARD) -oS -D UNIX_TIME=$(TIME) -o $(TARGET).elf $(SRC)

debug:
	$(CC) $(CSTANDARD) -oS -D UNIX_TIME=$(TIME) -g -o $(TARGET).elf $(SRC)
	echo $(TIME)

mspdebug:
	$(MSPDEBUG) $(MSPPROGRAMMER)

gdbdebug: debug
	$(MSPDEBUG) $(MSPPROGRAMMER) "erase"
	$(MSPDEBUG) $(MSPPROGRAMMER) "prog $(TARGET).elf" gdb

clean:
	rm $(TARGET).elf
