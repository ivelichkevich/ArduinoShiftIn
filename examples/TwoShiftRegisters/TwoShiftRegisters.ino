#include <ShiftIn.h>

BaseShiftIn* shift;

void displayValues() {
    for(int i = 0; i < shift->getDataWidth(); i++) { Serial.print(shift->state(i)); }
}

void setup() {
    Serial.begin(9600);
    shift = new ShiftIn<5>(8, 9, 11, 12);
}

void loop() {
    if(shift->update()) { displayValues(); } // read in all values. returns true if any button has changed
    delay(1);
}