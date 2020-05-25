CXXFLAGS=-Os -DF_CPU=9600000UL -mmcu=attiny13a
PROCESSOR=attiny13a
LDFLAGS=-mmcu=$(PROCESSOR)
CXX=avr-gcc
CC=avr-gcc
AS=avr-as
PROGRAMMER_PROCESSOR=t13

PROGNAME=main
COMPORT=/dev/ttyUSB0
BAUD=19200

$(PROGNAME).hex: $(PROGNAME)
	avr-objcopy -O ihex -R .eeprom $(PROGNAME) $(PROGNAME).hex

upload: $(PROGNAME).hex
	avrdude -v -cavrisp -p$(PROGRAMMER_PROCESSOR) -P$(COMPORT) -b$(BAUD) -Uflash:w:$(PROGNAME).hex:i

fuse:
	avrdude -V -cavrisp -p$(PROGRAMMER_PROCESSOR) -P$(COMPORT) -b$(BAUD) -Ulfuse:w:0x7a:m -Uhfuse:w:0xff:m

main.o: main.cc

$(PROGNAME): main.o

clean:
	$(RM) *.hex
	$(RM) *.o
	$(RM) $(PROGNAME)
