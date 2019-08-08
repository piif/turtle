// code related to user interface : led and button

#define BUTTON 2
#define LED 13

// state value : '0' = off , '1' = on , 'B' = blink slowly , 'E' = blick quickly (error state)
void setLed(char state) {
	switch(state) {
		case '1':
			digitalWrite(LED, HIGH);
			changeInterval(ledTimer, SET_INTERVAL_PAUSED);
		break;
		case '0':
			digitalWrite(LED, LOW);
			changeInterval(ledTimer, SET_INTERVAL_PAUSED);
		break;
		case 'B':
			changeInterval(ledTimer, 250);
		break;
		case 'E':
			changeInterval(ledTimer, 50);
		break;
	}
}

// timer callback to blink led
void updateLed(void *, long, int) {
	static byte ledState = LOW;
	ledState = !ledState;
	digitalWrite(LED, ledState);
}

// return true if button was clicked
// if button is pressed, function is blocking until it's released
bool click() {
	if (digitalRead(BUTTON) == HIGH) {
		return false;
	}
	do {
		delay(10);
	} while(digitalRead(BUTTON) == LOW);
	return true;
}
