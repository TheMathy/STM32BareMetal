CFLAGS  ?=  -W -Wall -Wextra -pedantic -fno-common \
            -g3 -Os -ffunction-sections -fdata-sections -I. \
            -mcpu=cortex-m3 -mthumb $(EXTRA_CFLAGS)
LDFLAGS ?= -Tlink.ld -nostartfiles -nostdlib --specs nano.specs -lc -lgcc -Wl,--gc-sections -Wl,-Map=$@.map
SOURCES = src/Reset.c src/Main.c src/Peripherals.c
INCLUDES = src/Peripherals.h


build: bin/firmware.elf


bin/firmware.elf: $(SOURCES) $(INCLUDES)
	@echo "Building: ..."
	@arm-none-eabi-gcc $(SOURCES) $(CFLAGS) $(LDFLAGS) -o $@
	@arm-none-eabi-objcopy -O binary $@ bin/firmware.bin
	@echo "Build finished"

clean:
	@echo "Cleaning"
	@rm -rf bin/firmware.*
