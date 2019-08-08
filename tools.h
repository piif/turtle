#ifndef DEFAULT_BAUDRATE
	#define DEFAULT_BAUDRATE 115200
#endif

#define XSTR(s) STR(s)
#define STR(s) #s

#define DEBUG(m) Serial.print(m)
#define DEBUGln(m) Serial.println(m)
