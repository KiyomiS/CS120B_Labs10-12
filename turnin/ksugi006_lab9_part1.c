/*	Author: ksugi006
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab 9  Exercise 1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
    
    TCCR1B = 0x0b;
    OCR1A = 125;
    TIMSK1 = 0x02;
    TCNT1 = 0;
    
    _avr_timer_cntcurr = _avr_timer_M;
    SREG |= 0x80;
}

void TimerOff() {
    TCCR1B = 0x00;
}

void TimerISR() {
    TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
    _avr_timer_cntcurr--;
    if(_avr_timer_cntcurr == 0) {
        TimerISR();
        _avr_timer_cntcurr = _avr_timer_M;
    }
}

void TimerSet(unsigned long M) {
    _avr_timer_M = M;
    _avr_timer_cntcurr = _avr_timer_M;
}

enum SM_State1 {one, two, three} state1;
enum SM_State2 {left, right} state2;
enum SM_state3 {start} state3;

unsigned char threeLEDs = 0x00;
unsigned char blinkingLED = 0x00;

unsigned char count = 0;
unsigned char val = 10;
void ThreeLEDsSM() {
	switch(state1) {
		case one:
			if (count == val) {
				state1 = two;
				count = 0;
			} else {
				state1 = one;
			}
			break;
		case two:
			if (count == val) {
				state1 = three;
				count = 0;
			} else {
				state1 = two;
			}
			break;
		case three:
			if (count == val) {
				state1 = one;
				count = 0;
			} else {
				state1 = three;
			}
			break;
		default:
			state1 = one;
			break;

	}
	switch(state1) {
		case one:
			count++;
			threeLEDs = 0x01;
			break;
		case two:
			count++;
			threeLEDs = 0x02;
			break;
		case three:
			count++;
			threeLEDs = 0x04;
			break;
		default:
			count = 0;
			threeLEDs = 0x00;
			break;
	}

}
unsigned char count1 = 0;
void BlinkingLEDSM() {
	switch(state2) {
		case left:
			if (count1 == val) {
				state2 = right;
				count1 = 0;
			} else {
				state2 = left;
			}
			break;
		case right:
			if (count1 == val) {
				state2 = left;
				count1 = 0;
			} else {
				state2 = right;
			}
			break;
		default:
			state2 = left;
			break;
	}
	switch(state2) {
		case left:
			count1++;
			blinkingLED = 0x08;
			break;
		case right:
			count1++;
			blinkingLED = 0x01;
			break;
		default:
			count1 = 0;
			blinkingLED = 0x00;
			break;
	}
}

void CombineLEDsSM() {
	switch(state3) {
		case start:
			PORTB = threeLEDs | blinkingLED;
			break;
		default:
			state3 = one;
			break;
	}

}
int main(void) {
	/* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;
    	

    TimerSet(100);
    TimerOn();
    PORTB = 0x00;
    /* Insert your solution below */
    while (1) {
        while (!TimerFlag);
        TimerFlag = 0;
	ThreeLEDsSM();
	BlinkingLEDSM();
	CombineLEDsSM();   	 
	}
    return 1;    

}



