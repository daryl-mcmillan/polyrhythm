#define FCPU 9600000UL
#include <avr/io.h>

uint16_t adc_read(uint8_t mux) {
  ADMUX = (0<<REFS0) // vcc reference
    | (1<<ADLAR) // left-adjusted values
    | (mux<<MUX0);
  ADCSRB = 0;
  ADCSRA = (1<<ADEN) // adc enable
    | (1<<ADSC)  // start conversion
    | (0<<ADATE) // auto trigger
    | (0<<ADIF)  // interrupt flag
    | (1<<ADIE)  // interrupt enable
    | (1<<ADPS2) // prescale division by 32
    | (0<<ADPS1)
    | (1<<ADPS0);
  while( ADCSRA & (1<<ADSC) ) { }
  return ADC;
}

int main(void) {

  DIDR0 = ( 1 << ADC3D ) | ( 1 << ADC2D );
  DDRB = 0b00000100;
  PORTB = 0b00000000;

  uint8_t clock_pin = 0b00000001;
  uint8_t out_pin   = 0b00000100;
  uint8_t last = 0;
  uint8_t count = 0;
  uint8_t clock_divider = 1;
  uint8_t pulse_duration = 1;
  for( ;; ) {
    uint8_t sample = PINB;
    uint8_t changed = last ^ sample;
    uint8_t leading_edge = changed & sample;
    uint8_t trailing_edge = changed & ~sample;
    last = sample;
    if( leading_edge & clock_pin ) {
      if( count == 0 ) {
        PORTB |= out_pin;
      }
      count ++;
      clock_divider = (adc_read(3) >> 10) + 1;
      pulse_duration = (adc_read(2) >> 10) + 1;
    }
    if( trailing_edge & clock_pin ) {
      if( count >= clock_divider ) {
        PORTB &= ~out_pin;
	count = 0;
      } else if( count >= pulse_duration ) {
        PORTB &= ~out_pin;
      }
    }
  }
}
