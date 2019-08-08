// code related to motor movement

// epsilon = 40 / (3 * PI) = 4.244131816 step/mm
// theta = (2 PI lambda / 360) * epsilon = (2 * 60 / 360) * 40 / 3 = 4.444 step/° thus 1600 steps for 360°
#define EPSILON_1000 4244
#define THETA_1000   4444

// D8..D11 == PORTB 0..3
// D4..D7  == PORTD 4..7
#define SET_MOTOR_1(step) PORTB = ( (PORTB & 0xF0) | (steps[step] & 0x0F) )
#define SET_MOTOR_2(step) PORTD = ( (PORTD & 0x0F) | (steps[step] << 4  ) )
#define SET_MOTOR(m, step) if(m) { SET_MOTOR_2(step); } else { SET_MOTOR_1(step); }

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


MotorState motors[2];
