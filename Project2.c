#ifndef F_CPU
#define F_CPU 1000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>

#define LED_PIN0 (1 << PB0)
#define LED_PIN1 (1 << PB1)
#define LED_PIN2 (1 << PB2)
#define KEY0 (1 << PB3)
#define KEY1 (1 << PB4)

typedef int bool;
#define true 1
#define false 0

void lockedState();

typedef enum { RELEASED=0, PRESSED } key_t;

volatile int code = 0b000000;

void startState(){
  volatile int pressCount = 0b000000;
  volatile uint8_t history = 0;
  volatile uint8_t history1 = 0;
  key_t keystate = RELEASED;
  DDRB = LED_PIN1|LED_PIN0;

  while(1){
    bool clicked = false;
    PORTB = LED_PIN0;
    PORTB = LED_PIN1;
    //PORTB = LED_PIN2;
  history = history << 1;
  if ((PINB & KEY0) == 0){ // low if button is pressed!
    history = history | 0x1;
  }

  // Update the key state based on the latest 6 samples
  if ((history & 0b111111) == 0b111111){
    keystate = PRESSED;
    clicked = true;
    _delay_ms(5);
  }


    if ((history & 0b00111111) == 0){
      keystate = RELEASED;
    }

    if (keystate == PRESSED && clicked){
      DDRB = LED_PIN1|LED_PIN2;
      PORTB = LED_PIN1;
      code = code << 1;
      code = code | 0b1;
      pressCount = pressCount << 1;
      pressCount = pressCount | 0b1;
      _delay_ms(175);
    }
      clicked = false;





      history1 = history1 << 1;
      if ((PINB & KEY1) == 0){ // low if button is pressed!
        history1 = history1 | 0x1;
      }

      // Update the key state based on the latest 6 samples
      if ((history1 & 0b111111) == 0b111111){
        keystate = PRESSED;
        clicked = true;
        _delay_ms(5);
      }


        if ((history1 & 0b00111111) == 0){
          keystate = RELEASED;
        }

        if (keystate == PRESSED && clicked){
          DDRB = LED_PIN1|LED_PIN2;
          PORTB = LED_PIN1;
          code = code << 1;
          code = code | 0b0;
          pressCount = pressCount << 1;
          pressCount = pressCount | 0b1;
          _delay_ms(175);
        }
          clicked = false;






    if (pressCount == 0b111111){
      DDRB = 0b000000;
      PORTB = 0b000000;
      lockedState();
    }
    DDRB = LED_PIN1|LED_PIN0;
    //_delay_ms(5);
  }
}
////////////////////////////////////////////////////////////////////////////////
void lockedState(){
  volatile int codeAttempt = 0b000000;
  volatile int pressCount = 0b000000;
  volatile uint8_t history = 0;
  volatile uint8_t history1 = 0;
  key_t keystate = RELEASED;
  DDRB = LED_PIN1 | LED_PIN2;
  PORTB = LED_PIN2;
  while(1){
    bool clicked = false;
    //PORTB = LED_PIN0;
    //PORTB = LED_PIN1;
    //PORTB = LED_PIN2;
  history = history << 1;
  if ((PINB & KEY0) == 0){ // low if button is pressed!
    history = history | 0x1;
  }

  // Update the key state based on the latest 6 samples
  if ((history & 0b111111) == 0b111111){
    keystate = PRESSED;
    clicked = true;
    _delay_ms(5);
  }


    if ((history & 0b00111111) == 0){
      keystate = RELEASED;
    }

    if (keystate == PRESSED && clicked){
      DDRB = LED_PIN1|LED_PIN2;
      PORTB = LED_PIN1;
      codeAttempt = codeAttempt << 1;
      codeAttempt = codeAttempt | 0b1;
      pressCount = pressCount << 1;
      pressCount = pressCount | 0b1;
      _delay_ms(175);
    }
      clicked = false;





      history1 = history1 << 1;
      if ((PINB & KEY1) == 0){ // low if button is pressed!
        history1 = history1 | 0x1;
      }

      // Update the key state based on the latest 6 samples
      if ((history1 & 0b111111) == 0b111111){
        keystate = PRESSED;
        clicked = true;
        _delay_ms(5);
      }


        if ((history1 & 0b00111111) == 0){
          keystate = RELEASED;
        }

        if (keystate == PRESSED && clicked){
          DDRB = LED_PIN1|LED_PIN2;
          PORTB = LED_PIN1;
          codeAttempt = codeAttempt << 1;
          codeAttempt = codeAttempt | 0b0;
          pressCount = pressCount << 1;
          pressCount = pressCount | 0b1;
          _delay_ms(175);
        }
          clicked = false;






    if (pressCount == 0b111111){
      if (codeAttempt == code){
        DDRB = 0b000000;
        PORTB = 0b000000;
        code = 0b000000;
        startState();
      } else {
        for (volatile int i = 0; i < 5; i++){
          DDRB = LED_PIN0|LED_PIN1;
          PORTB = LED_PIN1;
          _delay_ms(100);
          DDRB = 0b000000;
          PORTB = 0b000000;
          _delay_ms(100);
        }
        lockedState();
      }

    }
    DDRB = LED_PIN1 | LED_PIN2;
    PORTB = LED_PIN2;
    //_delay_ms(5);

  }
}

int main(void){
  while(1){
    startState();
  }
}
