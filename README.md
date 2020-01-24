# ArduinoShiftIn
This is a small library for reading in values from a 74HC165 8 bit shift register.

The Arduino has only a limited amount of pins, therefore it might happen that you run out of pins.
For this case, the 74HC165 comes to the rescue. It is an 8 bit shift register. It allows you to read in 8 values by only using 4 pins on your Arduino. Furthermore you can daisy-chain several shift registers by still only using 4 Arduino Pins.

## Easy install (zip import)
The easiest way to install this library is by downloading the newest release and then importing it.
You don't have to unzip it. Just open your Arduino IDE and navigate to *Sketch* > *Include Library* > *Add .ZIP* and then select the zip file.

## Manual install
Of course you can also install this library manually. To do so, download the newest release and unzip it. Then you have to copy the `ShiftIn` folder (NOT the `ShiftIn-x.y.z` folder) and copy it to your Arduino library folder:
* Windows: `My Documents\Arduino\libraries\`
* Mac and Linux: `Documents/Arduino/libraries/`

After that you just have to restart your Arduino IDE.

## Usage
If you have installed this library, you can include it by navigating to *Sketch* > *Include Library* > *ShiftIn*. This will add the line `#include <ShiftIn.h>` to your sketch (of course you could also write this line manually).

Now you can actually use this library:
``` c++
// Init ShiftIn instance with one chip.
#define SHIFT_REGISTER_CHIP_COUNT 1
#include <ShiftIn.h>

// declare pins: ploadPin, clockPin, dataPin, clockEnablePin
ShiftIn shift(8,12,11,9);

void setup()
{
  Serial.begin(115200);
}

void loop()
{
  shift.read();

  for (int n = 0; n < 8; ++n)
  {
    // Show state of each buttons
    if (shift.state(n))
    {
      Serial.print("1");
    }
    else
    {
      Serial.print("0");
    }
  }

  Serial.println("");
  delay(1000);
}
```
![Breadboard layout for one shift register](ShiftIn/examples/SingleShiftRegister/Layout1.png)

If you want to use two shift registers, you only have to change the declaration from `#define SHIFT_REGISTER_CHIP_COUNT 1` to `#define SHIFT_REGISTER_CHIP_COUNT 2`. The layout can look like this;
![Breadboard layout for two shift registers](ShiftIn/examples/TwoShiftRegisters/Layout2.png)

Gets/sets the delay time for the clock pin (in microseconds). This value defaults to 5 and in most cases there's no need to change it.
``` c++
uint8_t getPulseWidth()
void setPulseWidth(uint8_t value)
```

Returns true if button i has changed its state during the last frame.
``` c++
boolean hasChanged(int i)
```

Returns the state of a single button for this frame / last frame
``` c++
boolean state(int i)
boolean last(int i)
```

Whether button i is just pressed/released
``` c++
boolean pressed(int id) // wasn't pressed lasst frame, but is pressed now
boolean released(int id) // was pressed last frame, but is released now
```

This function (or the update function) should be called each frame exactly once. It will read in all values from the shift registers.
``` c++
void read()
```
