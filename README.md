# Tiny-AVR-Digitial-Lock

A digital lock using 'Tiny AVR Programmer' and 'ATtiny85'

Hardware: two momentary pushbuttons (key0 and key1), and four leds (one green, one red,
one blue, and one yellow).  Your hardware team has selected the ATtiny85,
which only has 5 usable I/O pins, and so there is multiplexing of the leds
using the Charlieplexing technique.

Initialization state:  At power up/reset, the unit should be in the
unlocked state (indicated by the green led) and in programming mode
(indicated by the yellow led).  It will then accept a 6 bit code, using
KEY0 and KEY1 to represent "0" and "1" respectively.  When the code is provided,
it should move into the locked state (indicated by the red
led), and the programming led should go off.

It will now wait for a 6 bit code to be entered.  If the entry is correct,
it will move to unlock/program mode.  If incorrect, it will flash the
yellow led for a short time, and wait for another 6 bit input.

The blue led should be used as a visual indicator to acknowledge a
button press (i.e. a short flash for each press).

Save the code to the EEPROM memory so that it persists even when powered
down.  In this case, if the last state was locked, it will power up into
the locked state, and will only unlock if the stored 6 bit key is entered.
Has a "secret" power up mode where if KEY1 is held down during power
up it will clear the stored value and start in the unlocked state.
