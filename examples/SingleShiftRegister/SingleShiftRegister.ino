#include <ShiftIn.h>

ShiftIn<1> shift(8, 9, 11, 12); // Init ShiftIn instance with a single chip

void displayValues() { // print out all 8 buttons
    for(int i = 0; i < shift.getDataWidth(); i++) Serial.print( shift.state(i) ); // get state of button i
    Serial.println();
}
void setup() { Serial.begin(9600); }
void loop() {
    if(shift.update()) displayValues(); // read in all values. returns true if any button has changed
    delay(1);
}