#include <Arduino.h>

#include "motors.h"
#include "tools.h"

// code related to motor movement

// s = step number per rotation = 40
#define MOTOR_STEPS 40
// d = motor axis diameter
// lamba = distance between center and wheel
#define LAMBDA 30
// epsilon = s / (PI * d) = 4.244131816 step/mm
#define EPSILON_1000 4244
// theta = (2 PI lambda / 360) * epsilon = (2 * 60 / 360) * (40 / 3) = 4.444 step/° thus 1600 steps for 360°
#define THETA_1000   4444

#define PEN 3
#define PEN_MIN 9
#define PEN_MAX 34

// D8..D11 == PORTB 0..3
// D4..D7  == PORTD 4..7
#define SET_MOTOR_LEFT  PORTB = ( (PORTB & 0xF0) | (steps[motorLeft.step] & 0x0F) )
#define SET_MOTOR_RIGHT PORTD = ( (PORTD & 0x0F) | (steps[motorRight.step] << 4  ) )
#define RELEASE_MOTORS  { PORTB = (PORTB & 0xF0); PORTD = (PORTD & 0x0F); }

#define STEP_A 0x08
#define STEP_B 0x02
#define STEP_C 0x04
#define STEP_D 0x01

byte steps[] = { STEP_A, STEP_A|STEP_B, STEP_B, STEP_B|STEP_C, STEP_C, STEP_C|STEP_D, STEP_D, STEP_D|STEP_A  };
#define MAX_STEPS (sizeof(steps) -1)

typedef struct motorState {
	bool reverse = false;
	byte step = 0;
} MotorState;

MotorState motorLeft, motorRight;

void setupMotors() {
	pinMode(4, OUTPUT);
	pinMode(5, OUTPUT);
	pinMode(6, OUTPUT);
	pinMode(7, OUTPUT);

	pinMode(8, OUTPUT);
	pinMode(9, OUTPUT);
	pinMode(10, OUTPUT);
	pinMode(11, OUTPUT);

	// COM2A = 0 , COM2B = 2 , WGM = 3 , CS = 7
	TCCR2A = 0x23;
	TCCR2B = 0x07;
	penUp();
}

unsigned long prepareMove(long len) {
	if (len < 0) {
		motorLeft.reverse = true;
		motorRight.reverse = true;
		len = -len;
	} else {
		motorLeft.reverse = false;
		motorRight.reverse = false;
	}
	return 1 + len * EPSILON_1000 / 1000;
}

unsigned long prepareTurn(long angle) {
	if (angle < 0) {
		motorLeft.reverse = true;
		motorRight.reverse = false;
		angle = -angle;
	} else {
		motorLeft.reverse = true;;
		motorRight.reverse = false;
	}
	return 1 + angle * THETA_1000 / 1000;
}

// data to compute bresenham (https://fr.wikipedia.org/wiki/Algorithme_de_trac%C3%A9_de_segment_de_Bresenham)
bool innerLeft;
unsigned long dx, dy, err;

unsigned long prepareArc(long radius, int angle) {
	if (radius < 0) {
		innerLeft = false;
		radius = -radius;
	} else {
		innerLeft = true;
	}
	motorLeft.reverse = false;
	motorRight.reverse = false;

	long Rin, Rout = radius + LAMBDA;
	if (radius < LAMBDA) {
		Rin = LAMBDA - radius;
		if (innerLeft) {
			motorLeft.reverse = true;
		} else {
			motorRight.reverse = true;
		}
	} else {
		Rin = radius - LAMBDA;
	}

	unsigned long Nout = MOTOR_STEPS * Rout * angle / 360;
	unsigned long Nin  = MOTOR_STEPS * Rin  * angle / 360;
	
	dx = Nout * 2;
	dy = Nin * 2;
	err = Nout;

	return 1;
}

void movePen(byte pos) {
	OCR2B = pos;
	pinMode(PEN, OUTPUT);
	delay(750);
	pinMode(PEN, INPUT);
}

void penUp() {
	movePen(PEN_MAX);
}

void penDown() {
	movePen(PEN_MIN);
}

void nextStep(MotorState *motor) {
	motor->step = (motor->step + (motor->reverse ? -1 : 1)) & MAX_STEPS;
}

void doSubStep(char command, unsigned long substep) {
	byte mask;

	switch(command) {
		case 'M':
		case 'T':
			if (substep == 1) {
				RELEASE_MOTORS;
				DEBUGln("-");
			} else {
				// next step for each motor
				nextStep(&motorLeft);
				nextStep(&motorRight);
				SET_MOTOR_LEFT;
				SET_MOTOR_RIGHT;
			}
		break;
		case 'C':
			if (substep == 1) {
				RELEASE_MOTORS;
			} else {
				if (innerLeft) {
					nextStep(&motorRight);
					SET_MOTOR_RIGHT;
				} else {
					nextStep(&motorLeft);
					SET_MOTOR_LEFT;
				}
				err -= dy;
				if (err <= 0) {
					if (innerLeft) {
						nextStep(&motorLeft);
						SET_MOTOR_LEFT;
					} else {
						nextStep(&motorRight);
						SET_MOTOR_RIGHT;
					}
					err += dx;
				}
			}
		break;
	}
}
