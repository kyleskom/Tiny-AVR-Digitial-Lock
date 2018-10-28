#ifndef F_CPU
#define F_CPU 1000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>

#define LED_PIN0 (1 << PB0)
#define LED_PIN1 (1 << PB1)
#define LED_PIN2 (1 << PB2)
#define KEY0 (1 << PB3)
#define KEY1 (1 << PB4)

typedef int bool;
#define true 1
#define false 0

void lockedState();
void clear();

typedef enum { RELEASED=0, PRESSED } key_t;

volatile uint8_t code = 0b000000; // The lock code. Set to volatile so compiler won't make changes

/* @return uint8_t
   @parameters bool
   Function to create a uint8_t that represents a 6 bit input pattern
   Uses debouncing and delays to hanlde switch bouncing
   Shifts code << 1, adds 0b0 if KEY0 is pushed, 0b1 if KEY1
   Uses uint8_t to keep track of inputed code and button press count
   Turns on BLUE LED when button is pressed to indicate a press
   Checks to see if pressCount = 0b111111, indicating 6 inputs and that we have our 6 bit code, returns
   Checks what state we are in and sets LED's accordingly
*/
uint8_t codeInput(bool lockFlag){
  uint8_t pressCount = 0b000000; //How many times inputs were pressed
  uint8_t enteredCode = 0b000000; // Our input code
  uint8_t b0_history = 0;
  uint8_t b1_history = 0;
  key_t keystate = RELEASED;
  while(1){
    bool clicked = false;

  //Button 0
  b0_history = b0_history << 1;
  if ((PINB & KEY0) == 0){ // low if button is pressed!
    b0_history = b0_history | 0x1;
  }

  // Update the key state based on the latest 6 samples
  if ((b0_history & 0b111111) == 0b111111){
    keystate = PRESSED;
    clicked = true;
    _delay_ms(5);
  }

    if ((b0_history & 0b00111111) == 0){
      keystate = RELEASED;
    }

    /* KEY0 PRESSED
       Set BLUE LED to on
       Shifts enteredCode << 1
       Adds 0b0 to least significant bit
       Increases pressCount via same method
    */
    if (keystate == PRESSED && clicked){
      DDRB = LED_PIN1|LED_PIN2;
      PORTB = LED_PIN1;
      enteredCode = enteredCode << 1;
      enteredCode = enteredCode | 0b0;
      pressCount = pressCount << 1;
      pressCount = pressCount | 0b1;
      _delay_ms(175);
    }
      clicked = false;

      // Button 1
      b1_history = b1_history << 1;
      if ((PINB & KEY1) == 0){ // low if button is pressed!
        b1_history = b1_history | 0x1;
      }

      // Update the key state based on the latest 6 samples
      if ((b1_history & 0b111111) == 0b111111){
        keystate = PRESSED;
        clicked = true;
        _delay_ms(5);
      }

        if ((b1_history & 0b00111111) == 0){
          keystate = RELEASED;
        }

        /* KEY1 PRESSED
           Set BLUE LED to on
           Shifts enteredCode << 1
           Adds 0b1 to least significant bit
           Increases pressCount via same method
        */
        if (keystate == PRESSED && clicked){
          DDRB = LED_PIN1|LED_PIN2;
          PORTB = LED_PIN1;
          enteredCode = enteredCode << 1;
          enteredCode = enteredCode | 0b1;
          pressCount = pressCount << 1;
          pressCount = pressCount | 0b1;
          _delay_ms(175);
        }
          clicked = false;

    /* Check to see if we entered 6 bit code
       If pressCount = 0b111111, we have entered our 6 bit input
       Clear LED's and return our 6 bit code
    */
    if (pressCount == 0b111111){
      clear();
      return enteredCode;
    }

    // Checks what state we are in and sets LED's accordingly
    if(lockFlag){
      DDRB = LED_PIN1 | LED_PIN2;
      PORTB = LED_PIN2;
    }else {
      DDRB = LED_PIN1|LED_PIN0;
      PORTB = LED_PIN0;
      PORTB = LED_PIN1;
    }
    //_delay_ms(5);
  }
}

/* Representation of the unlocked state
 Sets GREEN and YELLOW LED's to on
 Calls the function codeInput that creates our 6 bit code
 Calls lockedState which represents being locked. */
void startState(){
  DDRB = LED_PIN1|LED_PIN0;
  PORTB = LED_PIN0;
  PORTB = LED_PIN1;
  code = codeInput(false);
  lockedState();
}

/* Representation of the locked state
  Sets RED LED to on
  Gets a user inputed 6 bit code via codeInput
  Compares the lock code to the new user input
  If equal changes state to unlocked state, clears LED's
  Else, flashes YELLOW LED 5 times to signal invalid code
*/
void lockedState(){
  uint8_t codeAttempt = 0b000000;
  DDRB = LED_PIN1 | LED_PIN2;
  PORTB = LED_PIN2;
  codeAttempt = codeInput(true);

  if (codeAttempt == code){
    clear();
    code = 0b000000;
    startState();
  } else {
    for (volatile int i = 0; i < 5; i++){
      DDRB = LED_PIN0|LED_PIN1;
      PORTB = LED_PIN1;
      _delay_ms(100);
      clear();
      _delay_ms(100);
    }
    lockedState();
  }
}

// Clear DDRB & PORTB to turn off all LED's
void clear(){
  DDRB = 0b000000;
  PORTB = 0b000000;
}

//Entry point
int main(void){
    startState();
}
