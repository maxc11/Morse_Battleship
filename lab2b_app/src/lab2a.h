/*****************************************************************************
* lab2a.h for Lab2A of ECE 153a at UCSB
* Date of the Last Update:  October 23,2014
*****************************************************************************/

#ifndef lab2a_h
#define lab2a_h

enum Lab2ASignals {
	ENCODER_UP_SIG = Q_USER_SIG,
	ENCODER_DOWN_SIG,
	ENCODER_CLICK_SIG,
	BTN_SIG,
	BTN_TIMEOUT_SIG,
	TWIST_TIMEOUT_SIG,
	SHORT_PRESS_SIG,
	LONG_PRESS_SIG,
	TRANSLA_SIG,
	BTN_SIG_SELECTION
};


extern struct Lab2ATag AO_Lab2A;
extern int index1;
extern int index2;
extern int numTimes[18];
extern int cordinates[2];
extern int orientation;
void Lab2A_ctor(void);
void GpioHandler(void *CallbackRef);
void TwistHandler(void *CallbackRef);
char classify_morse(const char* morse_code);

#endif  
