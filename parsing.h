// code related to program parsing

// version 0.1.0 => 00 01 00 => 100
#define PROTOCOL_VERSION 100
#define MAX_PROGRAM 100

#define CMD_START '['
#define CMD_END   ']'
#define CMD_MOVE  'M'
#define CMD_TURN  'T'
#define CMD_UP    'U'
#define CMD_DOWN  'D'
#define CMD_ARC   'C'
#define CMD_WAIT  '#'

/**
- One line per instruction ended by \n (or \r\n or \r)
- [nn = start program, protocol version nn
  => stop current program if any
- ] = end program
  => store and wait button click to continue
- Mxxx = move of xxx
- Txxx = turn of xxx (>0 = trigo, <0 = clockwise)
  Example : T-90
- U = pen up
- D = pen down
- Cr[,a] with r = radius (>0 => to left, <0 +> to right) and a = angle (default 360°)
  arc circle to right/left
  Example : C-50,90
- # = pause (wait button click)
**/

typedef struct programLine {
	char command; // #, M, T, U, D, C
	long arg1;
	long arg2;
} ProgramLine;

extern ProgramLine program[];

// parse input to load a program
// returns :
// - 0 if no input
// - 1 if program loaded
// - >1 if error
byte handleInput(Stream &channel);
