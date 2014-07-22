PRG					= melbus_injector
OBJ					= melbus_injector.o
MCU_TARGET  = atmega328p
OPTIMIZE		= -Os
DEFS 				=
LIBS				=

HZ					= 16000000

# You should not have to change anything below here.

CC             = avr-gcc

# Override is only needed by avr-lib build system.

override CFLAGS        = -g -DF_CPU=$(HZ) -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS)
override LDFLAGS       = -Wl,-Map,$(PRG).map

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump

all: $(PRG).elf lst text

$(PRG).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf *.o $(PRG).elf *.eps *.png *.pdf *.bak *.hex *.bin *.srec
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)

lst:  $(PRG).lst

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

# Rules for building the .text rom images

text: hex bin srec

hex:  $(PRG).hex
bin:  $(PRG).bin
srec: $(PRG).srec

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@

%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@

# debug using simulavr

gdbserver: gdbinit
	simulavr --device $(MCU_TARGET) --gdbserver

gdbinit: $(GDBINITFILE)

$(GDBINITFILE): $(PRG).hex
	@echo "file $(PRG).elf" > $(GDBINITFILE)

	@echo "target remote localhost:1212" >> $(GDBINITFILE)
	@echo "load"                         >> $(GDBINITFILE)
	@echo "break main"                   >> $(GDBINITFILE)
	@echo
	@echo "Use 'avr-gdb -x $(GDBINITFILE)'"