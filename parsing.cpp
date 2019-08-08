#ifdef PIF_TOOL_CHAIN
	#include <Arduino.h>
#endif

#include "parsing.h"
#include "tools.h"
#include "intf.h"

ProgramLine program[MAX_PROGRAM] = { {']', 0, 0} };

// display parse error
void error(int line, char *message) {
	Serial.print("ERR line ");
	Serial.print(line);
	Serial.print(" : ");
	Serial.println(message);
}

byte eol(int line, Stream &channel) {
	byte result = 0;
	int c = channel.peek();
	if (c == ';') {
		channel.read();
		result= 1;
		c = channel.peek();
	}
	if (c == '\r') {
		channel.read();
		result= 1;
		c = channel.peek();
	}
	if (c == '\n') {
		channel.read();
		result= 1;
	}
	if (result == 0) {
		error(line, "missing newline");
		return 2;
	}
	return result;
}


// parse input to load a program
// returns :
// - 0 if no input
// - 1 if program loaded
// - >1 if error
byte handleInput(Stream &channel) {
	if (channel.available() == 0) {
		return 0;
	}

	setLed('0');
	int line = 0;

	for(;;) {
		int command = channel.read();

		if (line >= MAX_PROGRAM) {
			error(line, "program too long (max " XSTR(MAX_PROGRAM) ")");
			return 2;
		}

		ProgramLine *prg = &(program[line]);
		prg->command = (char)command;

		if (line == 0 && command != CMD_START) {
			error(line, "first line must start with " XSTR(CMD_START));
			return 2;
		}

		switch(command) {
			case CMD_START:
				if (line != 0) {
					error(line, XSTR(CMD_START) "must only be at first line");
					return 2;
				}
				prg->arg1 = channel.parseInt();
				if (prg->arg1 == 0) {
					error(line, "can't read version");
					return 2;
				} else if (prg->arg1 > PROTOCOL_VERSION) {
					error(line, "version must be less than "  XSTR(PROTOCOL_VERSION));
					return 2;
				}
			break;
			case CMD_MOVE:
				prg->arg1 = channel.parseInt();
			break;
			case CMD_TURN:
				prg->arg1 = channel.parseInt();
			break;
			case CMD_UP:
			break;
			case CMD_DOWN:
			break;
			case CMD_ARC:
				prg->arg1 = channel.parseInt();
				if (channel.peek() == ',') {
					channel.read();
					prg->arg2 = channel.parseInt();
				} else {
					prg->arg2 = 360;
				}
			break;
			case CMD_WAIT:
				break;
			case CMD_END:
				if(!eol(line, channel)) return 2;
				DEBUGln("INF : program end.");
				return 1;
			break;
			default:
				error(line, "bad command");
				DEBUGln(command);
				return 2;
			break;
		}
		if(!eol(line, channel)) return 2;

		DEBUG("INF : cmd "); DEBUG(prg->command);
		DEBUG(" "); DEBUG(prg->arg1);
		DEBUG(" "); DEBUGln(prg->arg2);

		line++;

		while(channel.available() == 0) {
			delay(100);
		}
	}
}
