CC=arm-none-eabi-gcc
CFLAGS=-mcpu=cortex-m4 -mthumb -O2 -g -Wall
LDFLAGS=-Tstm32f446re.ld -nostartfiles -Wl,--gc-sections

SRC=main.c startup_stm32f446xx.s
OBJ=$(SRC:.c=.o)

all: main.elf

main.elf: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) main.elf

.PHONY: all clean
