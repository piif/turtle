// code related to code execution

#define STEP_DELAY 1000

// next position in program , remaining sub-steps into it
int step = 0, substep = 0;

// timer callback to run program steps
void doStep(void *, long, int) {
	if (state != 'R') {
		return;
	}

	if (substep == 0) {
		// read next command
		ProgramLine *prg = &(program[step]);
		// deduce actions
		switch(prg->command) {
			case CMD_START:
				DEBUGln("pause before start");
				state = 'P';
				setLed('1');
			break;
			case CMD_MOVE:
				DEBUG("MOVE "); DEBUGln(prg->arg1);
			break;
			case CMD_TURN:
				DEBUG("TURN "); DEBUGln(prg->arg1);
			break;
			case CMD_UP:
				DEBUGln("UP");
			break;
			case CMD_DOWN:
				DEBUGln("DOWN");
			break;
			case CMD_ARC:
				DEBUG("CIRCLE "); DEBUG(prg->arg1); DEBUG(" , "); DEBUGln(prg->arg2);
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
		// todo ...
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
