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

#include <Arduino.h>

class ShiftIn {
protected:
    byte ploadPin;
    byte clockPin;
    byte dataPin;
    byte clockEnPin;
    const uint8_t regCount;
    const uint16_t dataWidth;
    uint8_t pulseWidth;
    byte* lastState;
    byte* currentState;
public:
    ShiftIn(int ploadPin, int clockPin, int dataPin, int clockEnPin, int regCount) : regCount(regCount), dataWidth(regCount << 3), pulseWidth(5) {
        pinMode(this->ploadPin = ploadPin, OUTPUT);      // Connects to Parallel load pin the 165
        pinMode(this->clockPin = clockPin, OUTPUT);      // Connects to the Clock pin the 165
        pinMode(this->dataPin = dataPin, INPUT); 	     // Connects to the Q7 pin the 165
        pinMode(this->clockEnPin = clockEnPin, OUTPUT);	 // Connects to Clock Enable pin the 165
        lastState = new byte[regCount];
        currentState = new byte[regCount];
    }
    virtual ~ShiftIn() {
        delete [] lastState;
        delete [] currentState;
    }
    inline uint8_t getPulseWidth() { return pulseWidth; }
    inline void setPulseWidth(uint8_t value) { pulseWidth = value; }
    inline uint16_t getDataWidth() { return dataWidth; }
    inline const byte* getCurrentState() { return currentState; }
    /** whether the value with index 'pinNum' has changed */
    inline boolean hasChanged(int pinNum) {
        int nReg = pinNum / 8;
        int nBit = pinNum % 8;
        return bitRead(lastState[nReg], nBit) != bitRead(currentState[nReg], nBit);
    }
    /** whether button 'pinNum' is pressed or not */
    inline boolean state(int pinNum) {
        int nReg = pinNum / 8;
        int nBit = pinNum % 8;
        return bitRead(currentState[nReg], nBit);
    }
    /** whether button 'pinNum' was pressed in the last frame */
    inline boolean last(int pinNum) {
        int nReg = pinNum / 8;
        int nBit = pinNum % 8;
        return bitRead(lastState[nReg], nBit);
    }
    /** whether button 'pinNum' is now pressed, but wasn't pressed in the last frame */
    inline boolean pressed(int pinNum) {
        int nReg = pinNum / 8;
        int nBit = pinNum % 8;
        return !bitRead(lastState[nReg], nBit) && bitRead(currentState[nReg], nBit);
    }
    /** whether button 'pinNum' is now released, but was pressed in the last frame*/
    inline boolean released(int pinNum) {
        int nReg = pinNum / 8;
        int nBit = pinNum % 8;
        return bitRead(lastState[nReg], nBit) && !bitRead(currentState[nReg], nBit);
    }
    /** read in data from shift register */
    void read() {
        memcpy(lastState, currentState, regCount);
        memset(currentState, 0, regCount);
        digitalWrite(clockEnPin, HIGH);
        digitalWrite(ploadPin, LOW);
        delayMicroseconds(pulseWidth);
        digitalWrite(ploadPin, HIGH);
        digitalWrite(clockEnPin, LOW);
        for(int nReg = 0; nReg < regCount; ++nReg) {
            for(int nBit = 0; nBit < 8; ++nBit) {
                byte value = digitalRead(dataPin);
                currentState[nReg] |= value << nBit;
                digitalWrite(clockPin, HIGH);
                delayMicroseconds(pulseWidth);
                digitalWrite(clockPin, LOW);
            }
        }
    }
};

#endif