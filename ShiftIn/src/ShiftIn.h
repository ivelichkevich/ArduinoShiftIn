/* ShiftIn.h - Arduino library that reads in values from an 8 bit shift register (74HC165).
 * You can daisy-chain several shift register in order to read in up to 64 buttons by only using 4 Arduino pins.
 *
 * Created by Henrik Heine, July 24, 2016
 *
LICENSE
The MIT License (MIT)
Copyright (c) 2016 Henrik
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

#ifndef ShiftIn_h
#define ShiftIn_h

#ifndef SHIFT_REGISTER_CHIP_COUNT
#error "SHIFT_REGISTER_CHIP_COUNT need to be set to the number of 74HC165 used"
#endif

#include "Arduino.h"

class ShiftIn
{
private:
	byte ploadPin;
	byte clockEnablePin;
	byte dataPin;
	byte clockPin;

	const uint16_t dataWidth;
	uint8_t pulseWidth;

	byte lastState[SHIFT_REGISTER_CHIP_COUNT];
	byte currentState[SHIFT_REGISTER_CHIP_COUNT];

public:
	ShiftIn(int pload_pin01, int clock_pin2, int data_pin09, int clockEN_pin15) : dataWidth(SHIFT_REGISTER_CHIP_COUNT * 8), pulseWidth(5)
	{
		pinMode(ploadPin = pload_pin01, OUTPUT);				// Connects to Parallel load pin the 165
		pinMode(clockPin = clock_pin2, OUTPUT);				// Connects to the Clock pin the 165
		pinMode(dataPin = data_pin09, INPUT);					// Connects to the Q7 pin the 165
		pinMode(clockEnablePin = clockEN_pin15, OUTPUT);	// Connects to Clock Enable pin the 165
	}

	inline uint8_t getPulseWidth() { return pulseWidth; }
	inline void setPulseWidth(uint8_t value) { pulseWidth = value; }
	inline uint16_t getDataWidth() { return dataWidth; }
	
	// whether the value with index 'pin_id' has changed
	inline boolean hasChanged(int pin_id)
	{
		int nChipID = pin_id / 8;
		int nBit = pin_id % 8;
		return bitRead(lastState[nChipID], nBit) != bitRead(currentState[nChipID], nBit); 
	}

	// whether button 'pin_id' is pressed or not
	inline boolean state(int pin_id)
	{
		int nChipID = pin_id / 8;
		int nBit = pin_id % 8;
		return bitRead(currentState[nChipID], nBit); 
	}

	inline const byte* GetStates()
	{
		return currentState;
	}

	// whether button 'pin_id' was pressed in the last frame
	inline boolean last(int pin_id)
	{
		int nChipID = pin_id / 8;
		int nBit = pin_id % 8;
		return bitRead(lastState[nChipID], nBit);
	}

	// whether button 'pin_id' is now pressed, but wasn't pressed in the last frame
	inline boolean pressed(int pin_id)
	{
		int nChipID = pin_id / 8;
		int nBit = pin_id % 8;
		return !bitRead(lastState[nChipID], nBit) && bitRead(currentState[nChipID], nBit);
	}

	// whether button 'pin_id' is now released, but was pressed in the last frame
	inline boolean released(int pin_id)
	{
		int nChipID = pin_id / 8;
		int nBit = pin_id % 8;
		return bitRead(lastState[nChipID], nBit) && !bitRead(currentState[nChipID], nBit);
	}

	// read in data from shift register
	void read()
	{
		for (int nChipID=dataWidth/8-1; nChipID >= 0; --nChipID)
		{
			lastState[nChipID] = currentState[nChipID];
			currentState[nChipID] = 0;
		}

		digitalWrite(clockEnablePin, HIGH);
		digitalWrite(ploadPin, LOW);
		delayMicroseconds(pulseWidth);
		digitalWrite(ploadPin, HIGH);
		digitalWrite(clockEnablePin, LOW);

		for(int nChipID = dataWidth/8-1; nChipID >= 0; nChipID--)
		{
			for(int nBitID = 7; nBitID >= 0; nBitID--)
			{
				byte value = digitalRead(dataPin);
				currentState[nChipID] |= value << nBitID;
				digitalWrite(clockPin, HIGH);
				delayMicroseconds(pulseWidth);
				digitalWrite(clockPin, LOW);
			}
		}
	}
};

#endif