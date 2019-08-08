// PIF_TOOL_CHAIN_OPTION: UPLOAD_OPTIONS := -c "raw,cr"
// PIF_TOOL_CHAIN_OPTION: EXTRA_LIBS := ArduinoTools
#ifdef PIF_TOOL_CHAIN
	#include <Arduino.h>
	#include "setInterval/setInterval.h"
#else
	#include "setInterval.h"
#endif

#ifndef DEFAULT_BAUDRATE
	#define DEFAULT_BAUDRATE 115200
#endif

#define XSTR(s) STR(s)
#define STR(s) #s

#define DEBUG(m) Serial.print(m)
#define DEBUGln(m) Serial.println(m)

setIntervalTimer stepTimer = SET_INTERVAL_ERROR;
setIntervalTimer ledTimer  = SET_INTERVAL_ERROR;

// 'W' = Waiting for program , 'E' = parse Error , 'R' = program Running , 'P' = program Paused
byte state = 'W';

#include "motors.ino"
#include "intf.ino"
#include "parsing.ino"
#include "runcode.ino"

void setup() {
	Serial.begin(DEFAULT_BAUDRATE);

	pinMode(BUTTON, INPUT_PULLUP);
	pinMode(LED, OUTPUT);

	pinMode(4, OUTPUT);
	pinMode(5, OUTPUT);
	pinMode(6, OUTPUT);
	pinMode(7, OUTPUT);

	pinMode(8, OUTPUT);
	pinMode(9, OUTPUT);
	pinMode(10, OUTPUT);
	pinMode(11, OUTPUT);

	ledTimer  = setInterval(SET_INTERVAL_PAUSED, updateLed, NULL);
	stepTimer = setInterval(SET_INTERVAL_PAUSED, doStep, NULL);
	Serial.println("setup ok, waiting for program input");
	setLed('E');
}

void loop() {
	switch(handleInput(Serial)) {
		case 0:
			break;
		case 1:
			// new program loaded
			DEBUGln("new program loaded");
			state = 'R';
			setLed('0');
			startProgram();
			break;
		default:
			// program loading failed
			DEBUGln("parse error");
			state = 'E';
			setLed('E');
			stopProgram();
			break;
	}
	if (click()) {
		switch(state) {
			case 'R':
				// restart current program
				stopProgram();
				startProgram();
			break;
			case 'P':
				// unpause
				state = 'R';
				setLed('0');
			break;
		}
	}

	setIntervalStep();
}
