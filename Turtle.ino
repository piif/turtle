// PIF_TOOL_CHAIN_OPTION: UPLOAD_OPTIONS := -c "raw,cr"
// PIF_TOOL_CHAIN_OPTION: EXTRA_LIBS := ArduinoLibs ArduinoTools
#ifdef PIF_TOOL_CHAIN
	#include <Arduino.h>
	#include "setInterval/setInterval.h"
#else
	#include "setInterval.h"
#endif

#define DEMO_MODE
#ifdef DEMO_MODE
	#include "demo.h"
	#define INPUT DemoProgram
#else
	#define INPUT Serial
#endif

setIntervalTimer stepTimer = SET_INTERVAL_ERROR;

// 'W' = Waiting for program , 'E' = parse Error , 'R' = program Running , 'P' = program Paused
byte state = 'W';

#include "tools.h"
#include "parsing.h"
#include "motors.h"
#include "intf.h"

#define STEP_DELAY 8

// next position in program , remaining sub-steps into it
int step = 0;
unsigned long substep = 0;

ProgramLine *cmd = NULL;

// timer callback to run program steps
void doStep(void *, long, int) {
	if (state != 'R') {
		return;
	}

	if (substep == 0) {
		// read next command
		cmd = &(program[step]);

		// deduce actions
		switch(cmd->command) {
			case CMD_START:
				DEBUGln("pause before start");
				state = 'P';
				setLed('1');
			break;
			case CMD_MOVE:
				DEBUG("MOVE: "); DEBUGln(cmd->arg1);
				substep = prepareMove(cmd->arg1);
				DEBUG("move => substeps: "); DEBUGln(substep);
			break;
			case CMD_TURN:
				DEBUG("TURN: "); DEBUGln(cmd->arg1);
				substep = prepareTurn(cmd->arg1);
				DEBUG("turn => substeps: "); DEBUGln(substep);
			break;
			case CMD_UP:
				DEBUGln("UP");
				penUp();
			break;
			case CMD_DOWN:
				DEBUGln("DOWN");
				penDown();
			break;
			case CMD_ARC:
				DEBUG("CIRCLE "); DEBUG(cmd->arg1); DEBUG(" , "); DEBUGln(cmd->arg2);
				substep = prepareArc(cmd->arg1, cmd->arg2);
			break;
			case CMD_WAIT:
				DEBUGln("WAIT");
				state = 'P';
				setLed('B');
			break;
			case CMD_END:
				// end of program => restart.
				DEBUGln("end.");
				step = 0;
				return;
		}

		// prepare for next command
		step++;
	} else {
		doSubStep(cmd->command, substep);
		substep--;
	}
}

void startProgram() {
	step = 0;
	substep = 0;
	changeInterval(stepTimer, STEP_DELAY);
}

void stopProgram() {
	changeInterval(stepTimer, SET_INTERVAL_PAUSED);
}

void setup() {
	Serial.begin(DEFAULT_BAUDRATE);

	setupMotors();
	setupIntf();

	stepTimer = setInterval(SET_INTERVAL_PAUSED, doStep, NULL);
	Serial.println("setup ok, waiting for program input");
	setLed('E');
}

void loop() {
	switch(handleInput(INPUT)) {
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
