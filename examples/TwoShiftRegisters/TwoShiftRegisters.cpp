#include <ShiftIn.h>

ShiftIn shift(8,12,11,9, 2);

void setup() { Serial.begin(115200); }

void loop() {
	shift.read();
	for (int n = 0; n < 16; ++n) {
		if (shift.state(n)) { Serial.print("1"); }
		else { Serial.print("0"); }
	}
	Serial.println("");
	delay(1000);
}