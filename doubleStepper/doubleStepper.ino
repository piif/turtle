// PIF_TOOL_CHAIN_OPTION: EXTRA_LIBS := ArduinoTools
#ifdef PIF_TOOL_CHAIN
	#include <Arduino.h>
	#include "serialInput/serialInput.h"
#else
	#include "serialInput.h"
#endif

#ifndef DEFAULT_BAUDRATE
	#define DEFAULT_BAUDRATE 115200
#endif

// D8..D11 == PORTB 0..3
// D4..D7  == PORTD 4..7
// #define POLARITY ~
#define POLARITY
#define SET_MOTOR_1(bits) PORTB = ( (PORTB & 0xF0) | (POLARITY (bits) & 0x0F) )
#define SET_MOTOR_2(bits) PORTD = ( (PORTD & 0x0F) | (POLARITY (bits) << 4  ) )
#define SET_MOTOR(m, bits) if(m) { SET_MOTOR_2(bits); } else { SET_MOTOR_1(bits); }


#define STEP_A 0x01
#define STEP_B 0x04
#define STEP_C 0x02
#define STEP_D 0x08

byte fullSteps[] = { STEP_A, STEP_B, STEP_C, STEP_D  };
byte halfSteps[] = { STEP_A, STEP_A|STEP_B, STEP_B, STEP_B|STEP_C, STEP_C, STEP_C|STEP_D, STEP_D, STEP_D|STEP_A  };

typedef struct motorState {
	bool rotateRight = true;
	int remaining = 0;          // how many step to do before stop
	int stepDelay = 1000;       // how many ms between steps
	unsigned long nextStep = 0; // time in millis before next step
	byte step = 0;              // last position in step list
	byte mask = 0;              // last 4 bits mask sent to motor
} MotorState;

bool remanent = false; // keep current on between steps, or just send pulses
bool full;             // send half/full steps
byte maxSteps;
byte *steps;

MotorState motors[2];

void setMask(byte m, int d) {
	motors[m].remaining = -1;
	Serial.print(" -> ");
	Serial.println(d);
	SET_MOTOR(m, d);
}

void setNumber(byte m, int d) {
	if (d > 0) {
		motors[m].remaining = d;
		motors[m].rotateRight = true;
	} else {
		motors[m].remaining = -d;
		motors[m].rotateRight = false;
	}
}

void setSpeed(byte m, int d) {
	motors[m].stepDelay = 1000 / d;
}

void setMask1(int d) { setMask(0, d); }
void setNumber1(int d) { setNumber(0, d); }
void setSpeed1(int d) { setSpeed(0, d); }
void setMask2(int d) { setMask(1, d); }
void setNumber2(int d) { setNumber(1, d); }
void setSpeed2(int d) { setSpeed(1, d); }

void setHalf() {
	full = false;
	steps = halfSteps;
	maxSteps = sizeof(halfSteps) - 1;
}

void setFull() {
	full = true;
	steps = fullSteps;
	maxSteps = sizeof(fullSteps) - 1;
}

void setRemanent() {
	remanent = !remanent;
	if (remanent) {
		SET_MOTOR_1(motors[0].mask);
		SET_MOTOR_2(motors[1].mask);
	} else {
		SET_MOTOR_1(0);
		SET_MOTOR_2(0);
	}
}

void motorStatus(byte m) {
	Serial.print("Motor ");
	Serial.println(m+1);
	Serial.print(" speed : ");
	Serial.print(1000 / motors[m].stepDelay);
	Serial.print(" , remaining steps : ");
	Serial.println(motors[m].remaining);
}

void status() {
	Serial.println("Commands :");
	Serial.println("? : status");
	Serial.println("n v: motor 1, launch v steps (<0 for backward");
	Serial.println("N v: motor 2, launch v steps (<0 for backward");
	Serial.println("s v: motor 1, set speed (1-1000 step/s)");
	Serial.println("S v: motor 2, set speed");
	Serial.println("r : set/unset remanent");
	Serial.println("h : set half");
	Serial.println("f : set full");

	Serial.println("\nCurrent state :");
	if (remanent) {
		Serial.print(" remanent ,");
	}
	Serial.print(full ? " full" : "half");
	Serial.println(" steps");
	motorStatus(0);
	motorStatus(1);
	Serial.print("\nPorts :\n B = ");
	Serial.println(PORTB, BIN);
	Serial.print(" D = ");
	Serial.println(PORTD, BIN);
}

void doStep(byte m) {
	MotorState *motor = &(motors[m]);

	if (motor->remaining < 0) {
		return;
	}

	unsigned long now = millis();
	if (now >= motor->nextStep) {
		if (motor->remaining == 0) {
			if (!remanent) {
				SET_MOTOR(m, 0);
			}
			motor->remaining--;
			return;
		}

		motor->step = (motor->step + (motor->rotateRight ? 1 : -1)) & maxSteps;
		motor->mask = steps[motor->step];

		Serial.print(motor->step);
		Serial.print(" -> ");
		Serial.println(motor->mask);
		SET_MOTOR(m, motor->mask);

		motor->remaining--;
		motor->nextStep = now + motor->stepDelay;
	}
}

InputItem inputs[] = {
	{ '?', 'f', (void *)status },
	{ 'm', 'I', (void *)setMask1 },
	{ 'n', 'I', (void *)setNumber1 },
	{ 's', 'I', (void *)setSpeed1  },
	{ 'M', 'I', (void *)setMask2 },
	{ 'N', 'I', (void *)setNumber2 },
	{ 'S', 'I', (void *)setSpeed2  },
	{ 'r', 'f', (void *)setRemanent },
	{ 'h', 'f', (void *)setHalf },
	{ 'f', 'f', (void *)setFull }
};

void setup() {
	Serial.begin(DEFAULT_BAUDRATE);

	setFull();
//	pinMode(A2, OUTPUT);
//	pinMode(A3, OUTPUT);
//	pinMode(A4, OUTPUT);
//	pinMode(A5, OUTPUT);

	pinMode(4, OUTPUT);
	pinMode(5, OUTPUT);
	pinMode(6, OUTPUT);
	pinMode(7, OUTPUT);

	pinMode(8, OUTPUT);
	pinMode(9, OUTPUT);
	pinMode(10, OUTPUT);
	pinMode(11, OUTPUT);

	registerInput(sizeof(inputs), inputs);
	Serial.println("setup ok");
}

void loop() {
	handleInput();
	doStep(0);
	doStep(1);
}
