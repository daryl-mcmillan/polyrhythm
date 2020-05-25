#define FCPU 9600000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

static volatile uint8_t adc0;
static volatile uint8_t adc1;

void adc_setup() {
  DIDR0 = ( 1 << ADC3D );
  ADMUX = (0<<REFS0) // vcc reference
    | (1<<ADLAR) // left-adjusted values
    | (1<<MUX1)
    | (1<<MUX0);
  ADCSRB = 0;
  ADCSRA = (1<<ADEN) // adc enable
    | (1<<ADSC)  // start conversion
    | (0<<ADATE) // auto trigger
    | (0<<ADIF)  // interrupt flag
    | (1<<ADIE)  // interrupt enable
    | (1<<ADPS2) // prescale division by 32
    | (0<<ADPS1)
    | (1<<ADPS0);
  sei();
}

ISR(ADC_vect) {
  if( ADMUX & (1<<MUX0) ) {
    adc0 = ADC >> 8;
  } else {
    adc1 = ADC >> 8;
  }
  ADMUX ^= (1<<MUX0); // toggle ADC2/ADC3
  ADCSRA |= (1<<ADSC);
}

int main(void) {

  DDRB = 0b00000100;
  PORTB = 0b00000000;

  adc_setup();

  DIDR0 = ( 1 << ADC3D );
  ADMUX = (0<<REFS0)
    | (1<<ADLAR)
    | (1<<MUX1)
    | (1<<MUX0);
  ADCSRB = 0;
  ADCSRA = (1<<ADEN)
    | (1<<ADSC)
    | (1<<ADATE)
    | (0<<ADIF)
    | (1<<ADIE)
    | (1<<ADPS2)
    | (0<<ADPS1)
    | (1<<ADPS0);
  sei();

  uint8_t clock_pin = 0b00000001;
  uint8_t out_pin   = 0b00000100;
  uint8_t last = 0;
  uint8_t count = 0;
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
    }
    if( trailing_edge & clock_pin ) {
      if( count >= (adc0>>2)+1 ) {
        PORTB &= ~out_pin;
	count = 0;
      } else if( count >= (adc1>>2)+1 ) {
        PORTB &= ~out_pin;
      }
    }
  }
}
