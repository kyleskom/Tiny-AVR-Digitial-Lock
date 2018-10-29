#ifndef F_CPU
#define F_CPU 1000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

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

uint8_t savedCode = 0b111111; // EEPROM memory where we save the state
uint8_t savedState = 0b111110; // EEPROM memory where we save the code

uint8_t code = 0b000000; // The lock code. Set to volatile so compiler won't make changes

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
      PORTB = LED_PIN1;
      _delay_ms(5);
      PORTB = LED_PIN0; // Using delays here to get full power to each LED to be brighter
      _delay_ms(5);
    }
  }
}

/* Representation of the unlocked state
 Sets GREEN and YELLOW LED's to on
 Calls the function codeInput that creates our 6 bit code
 Saves state and code to EEPROM
 Calls lockedState which represents being locked. */
void startState(){
  DDRB = LED_PIN1|LED_PIN0;
  PORTB = LED_PIN0;
  PORTB = LED_PIN1;
  code = codeInput(false);
  savedCode = code;
  eeprom_write_byte(&savedCode, code);
  eeprom_write_byte(&savedState, 0b100000);
  _delay_ms(75);
  lockedState();
}

/* Representation of the locked state
  Sets RED LED to on
  Gets a user inputed 6 bit code via codeInput
  Compares the lock code to the new user input
  If equal changes state to unlocked state, clears LED's
  Saves state and code to EEPROM
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
    savedCode = code;
    eeprom_update_byte(&savedCode, code);
    eeprom_update_byte(&savedState, 0b000000);
    _delay_ms(75);
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

/*Entry point
  If button is held down on startup will reset pin
  Checks to see if last state was a locked state
  If was locked, loads code from EEPROM memory and sets state to locked
*/
int main(void){
    uint8_t history = 0;
    key_t keystate = RELEASED;

    //If button is held down on startup, it will reset code.
    for(int i =0 ; i <1000;i++){
      // Get a sample of the key pin
      history = history << 1;
      if ((PINB & KEY1) == 0) // low if button is pressed!
        history = history | 0x1;

      // Update the key state based on the latest 6 samples
      if ((history & 0b111111) == 0b111111){
        keystate = PRESSED;
      }

      // Turn on the LED based on the state
      if (keystate == PRESSED){
        _delay_ms(2000);
        startState();
      }
    }

  /*Checks to see if we were last in a lockedState
    If we were we load that code into code
    Return to a locked state with the perviously used code
  */
  if(eeprom_read_byte(&savedState) == 0b100000){
    code = eeprom_read_byte(&savedCode);
    lockedState();
  } // Else we just enter the startState and ready to accept input
    startState();
}
