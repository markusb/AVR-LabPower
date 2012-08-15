#
# Makfile for Lab Power
#

PRG            = labpower
OBJ            = labpower.o lcd.o switch.o util.o uart.o adc.o dac.o st7565-driver.o st7565-graphics.o build.o
PROGRAMMER     = dragon_pdi
PORT           = usb
#MCU_TARGET     = atxmega32a4u
MCU_TARGET     = atxmega16a4u
AVRDUDE_TARGET = x16a4
OPTIMIZE       = -Os
#INCLUDES	   = -I /usr/local/asf-3.3.0 -I /usr/local/asf-3.3.0/xmega/utils -I /usr/local/asf-3.3.0/common/utils -I /usr/local/asf-3.3.0/xmega/utils/preprocessor
INCLUDES	   =
DEFS           = -D F_CPU=32000000
LIBS           = -L Dogm/utility

F_CPU          = 32000000

# You should not have to change anything below here.

CC             = avr-gcc

# Override is only needed by avr-lib build system.

#override CFLAGS        = -g -DF_CPU=$(F_CPU) -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS) $(INCLUDES)
override CFLAGS        = -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS) $(INCLUDES)
override LDFLAGS       = -Wl,-Map,$(PRG).map

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump

# DOGDEFS = -DDOGM132_HW
DOGDEFS = -DNHD12865AR_HW

# assume ATMEGA SPI interface on the target controller, default is Arduino Hardware
DOGDEFS += -DDOG_SPI_ATXMEGA

# Assign port and bit for the chip select line of the DOG controller
# this must be SS pin of the SPI interface. The chip select signal of
# the DOG controller must be connected to this SS pin
DOGDEFS += -DDOG_SPI_SS_PORT=PORTE -DDOG_SPI_SS_PIN=0
# assign port and bit for the clock of the DOG controller (this must be the SCL output of the USI interface)
DOGDEFS += -DDOG_SPI_SCL_PORT=PORTC -DDOG_SPI_SCL_PIN=7
# assign port and bit for the data line of the DOG controller (this must be the DO output of the USI interface)
DOGDEFS += -DDOG_SPI_MOSI_PORT=PORTC -DDOG_SPI_MOSI_PIN=5
# Assign port and bit for the adress line of the DOG controller
# The address line can by any port bit
DOGDEFS += -DDOG_SPI_A0_PORT=PORTE -DDOG_SPI_A0_PIN=1
DOGDEFS += -DDOG_SPI_CTRL=SPIC_CTRL -DDOG_SPI_DATA=SPIC_DATA -DDOG_SPI_STATUS=SPIC_STATUS

# DEFS += $(DOGDEFS)

all: $(PRG).elf lst text #eeprom

$(PRG).elf: $(OBJ)
	./build-commit.sh
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)
	avr-size $(PRG).elf

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
	avr-size $(PRG).elf

%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@

# Rules for building the .eeprom rom images

eeprom: ehex ebin esrec


ehex:  $(PRG)_eeprom.hex
#ebin:  $(PRG)_eeprom.bin
esrec: $(PRG)_eeprom.srec

%_eeprom.hex: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@

#%_eeprom.srec: %.elf
#	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O srec $< $@

%_eeprom.bin: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O binary $< $@


# command to program chip (invoked by running "make install")
install:  $(PRG).hex
	avrdude -q -p $(AVRDUDE_TARGET) -c $(PROGRAMMER) -P $(PORT) \
         -U flash:w:$(PRG).hex

fuse:
	avrdude -p $(AVRDUDE_TARGET) -c $(PROGRAMMER) -P $(PORT) -v \
	#-U lfuse:w:0xc6:m -U hfuse:w:0xd9:m

ddd: gdbinit
	ddd --debugger "avr-gdb -x $(GDBINITFILE)"

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

grep:
	@echo grep  /usr/local/avr/avr/include/avr/iox16a4u.h

