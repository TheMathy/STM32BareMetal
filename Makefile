CFLAGS  ?=  -W -Wall -Wextra -pedantic -Wno-unused-parameter -fno-common \
            -g3 -Os -ffunction-sections -fdata-sections -I src \
            -mcpu=cortex-m3 -mthumb $(EXTRA_CFLAGS)
LDFLAGS ?= -Tlink.ld -nostartfiles -specs=nano.specs -specs=nosys.specs -lc -lgcc -Wl,--gc-sections -Wl,-Map=$@.map
SOURCES = src/Reset.c src/Main.c src/Peripherals/Peripherals.c src/Peripherals/UART.c src/Peripherals/DMA.c
INCLUDES = src/Peripherals/Peripherals.h src/Peripherals/UART.h


build: bin/firmware.elf


bin/firmware.elf: $(SOURCES) $(INCLUDES)
	@echo "Building: ..."
	@arm-none-eabi-gcc $(SOURCES) $(CFLAGS) $(LDFLAGS) -o $@
	@arm-none-eabi-objcopy -O binary $@ bin/firmware.bin
	@echo "Build finished"

clean:
	@echo "Cleaning"
	@rm -rf bin/firmware.*
