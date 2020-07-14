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
#include "Arduino.h"

struct BaseShiftIn {
    virtual uint8_t getPulseWidth() = 0;
    virtual void setPulseWidth(uint8_t value) = 0;
    virtual uint16_t getDataWidth() = 0;
    /** whether some value has changed */
    virtual boolean hasChanged() = 0;
    /** whether the value with index 'nBit' has changed */
    virtual boolean hasChanged(int nBit) = 0;
    /** whether button 'nBit' is pressed or not */
    virtual boolean state(int nBit) = 0;
    /** whether button 'nBit' was pressed in the last frame */
    virtual boolean last(int nBit) = 0;
    /** whether button 'nBit' is now pressed, but wasn't pressed in the last frame */
    virtual boolean pressed(int nBit) = 0;
    /** whether button 'nBit' is now released, but was pressed in the last frame */
    virtual boolean released(int nBit) = 0;
    /** same as read, but it returns whether something has changed or not */
    virtual boolean update() = 0;
};

template<byte chipCount, typename ShiftType> class _ShiftIn : public BaseShiftIn {
protected:
    byte ploadPin;
    byte clockEnablePin;
    byte dataPin;
    byte clockPin;
    uint16_t dataWidth;
    uint8_t pulseWidth = 5;
    ShiftType lastState = 0;
    ShiftType currentState = 0;
    /** read in data from shift register and return the new value */
    ShiftType read() {
        lastState = currentState;
        ShiftType result = 0;
        digitalWrite(clockEnablePin, HIGH);
        digitalWrite(ploadPin, LOW);
        delayMicroseconds(pulseWidth);
        digitalWrite(ploadPin, HIGH);
        digitalWrite(clockEnablePin, LOW);
        for(int i = 0; i < dataWidth; i++) {
            ShiftType value = digitalRead(dataPin);
            result |= (value << ((dataWidth-1) - i));
            digitalWrite(clockPin, HIGH);
            delayMicroseconds(pulseWidth);
            digitalWrite(clockPin, LOW);
        }
        currentState = result;
        return result;
    }
public:
    _ShiftIn(int pload, int clockEN, int data, int clock) : dataWidth(chipCount * 8) {
        pinMode(ploadPin = pload, OUTPUT);
        pinMode(clockEnablePin = clockEN, OUTPUT);
        pinMode(dataPin = data, INPUT);
        pinMode(clockPin = clock, OUTPUT);
    }
    uint8_t getPulseWidth() override { return pulseWidth; }
    void setPulseWidth(uint8_t value) override { pulseWidth = value; }
    uint16_t getDataWidth() override { return dataWidth; }
    boolean hasChanged() override { return lastState != currentState; }
    boolean hasChanged(int nBit) override { return state(nBit) != last(nBit); }
    boolean state(int nBit) override { return bitRead(currentState, nBit); }
    boolean last(int nBit) override { return bitRead(lastState, nBit); }
    boolean pressed(int nBit) override { return !last(nBit) && state(nBit); }
    boolean released(int nBit) override { return last(nBit) && !state(nBit); }
    boolean update() override { return read() != lastState; }
    /** returns the state from the last frame */
    inline ShiftType getLast() { return lastState; }
    /** returns the current state */
    inline ShiftType getCurrent() { return currentState; }
};
/** fallback with 64 bit state (up to 8 shift registers in chipCount param) */
template<byte chipCount> class ShiftIn : public _ShiftIn<chipCount, uint64_t> {
    public: ShiftIn(int pload, int clockEN, int data, int clock) : _ShiftIn<chipCount, uint64_t>(pload, clockEN, data, clock) {}
};
template<> class ShiftIn<1> : public _ShiftIn<1, uint8_t>  {public: ShiftIn(int pload, int clockEN, int data, int clock):_ShiftIn(pload, clockEN, data, clock){}};
template<> class ShiftIn<2> : public _ShiftIn<2, uint16_t> {public: ShiftIn(int pload, int clockEN, int data, int clock):_ShiftIn(pload, clockEN, data, clock){}};
template<> class ShiftIn<3> : public _ShiftIn<3, uint32_t> {public: ShiftIn(int pload, int clockEN, int data, int clock):_ShiftIn(pload, clockEN, data, clock){}};
template<> class ShiftIn<4> : public _ShiftIn<4, uint32_t> {public: ShiftIn(int pload, int clockEN, int data, int clock):_ShiftIn(pload, clockEN, data, clock){}};

BaseShiftIn* shiftInFactory(byte n, byte pload, byte clkEN, byte dataIn, byte clock) {
    switch (n) {
        case 1: return new ShiftIn<1>(pload, clkEN, dataIn, clock);
        case 2: return new ShiftIn<2>(pload, clkEN, dataIn, clock);
        case 3: return new ShiftIn<3>(pload, clkEN, dataIn, clock);
        case 4: return new ShiftIn<4>(pload, clkEN, dataIn, clock);
        case 5: return new ShiftIn<5>(pload, clkEN, dataIn, clock);
        case 6: return new ShiftIn<6>(pload, clkEN, dataIn, clock);
        case 7: return new ShiftIn<7>(pload, clkEN, dataIn, clock);
        case 8: return new ShiftIn<8>(pload, clkEN, dataIn, clock);
    }
}
#endif