/*	Author: ksugi006
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab 9  Exercise 4
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>

#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
    
    TCCR1B = 0x0b;
    OCR1A = 13;//125; 125 is 1ms. 13 should be 1/10th of a ms
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
enum SM_state4 {on, off} state4;
enum SM_state5 {START, up, down, wait} state5;

unsigned char threeLEDs = 0x00;
unsigned char blinkingLED = 0x00;
unsigned char speakerSound = 0x00;


unsigned short count = 0;
unsigned short val = 3000;
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
unsigned short count1 = 0;
unsigned short val1 = 10000;
void BlinkingLEDSM() {
	switch(state2) {
		case left:
			if (count1 == val1) {
				state2 = right;
				count1 = 0;
			} else {
				state2 = left;
			}
			break;
		case right:
			if (count1 == val1) {
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
			PORTB = threeLEDs | blinkingLED | speakerSound;
			break;
		default:
			state3 = one;
			break;
	}

}

unsigned char count2 = 0;
unsigned char freq = 0;
unsigned char speakerFreq[] = {38, 34, 30, 28, 25,  22, 20, 18};

void toggleSound() {
	switch(state4) {
		case on:
			if  (count2 == speakerFreq[freq]) {
				state4 = off;
				count2 = 0;
			} else {
				state4 = on;
			}
			break;
		case off:
			if ( ((~PINA & 0x04) == 0x04) && (count2 == speakerFreq[freq]) ) {
				state4 = on;
				count2 = 0;
			} else {
				state4 = off;
			}
			break;
		default:
			state4 = off;
			break;
	}
	switch(state4) {
		case on:
			count2++;
			speakerSound = 0x10;
			break;
		case off:
			count2++;
			speakerSound = 0x00;
			break;
		default:
			speakerSound = 0x00;
			break;
	}

}

unsigned char count3 = 0;
unsigned short val2 = 500;
void adjustFreq() {
	switch(state5) {
		case START:
			if ((~PINA & 0x03) == 0x01) {
				state5 = up;
			} else if ((~PINA & 0x03) == 0x02) {
				state5 = down;
			} else {
				state5 = START;
			}
			break;
		case up:
			state5 = wait;
			break;
		case down:
			state5 = wait;
			break;
		case wait:
			if (count3 < 50) {
				state5 = wait;
			} else {
				if ((~PINA & 0x03) == 0x00) {
					state5 = START;
				} else {
					state5 = wait;
				}
			}
			break;
		default:
			count3 = 0;
			state5 = START;
			break;
	}
	switch(state5) {
		case START:
			count3 = 0;
			break;
		case up:
			if (freq < 7) {
				freq++;
			}
			break;
		case down:
			if (freq > 0) {
				freq--;
			}
			break;
		case wait:
			count3++;
			break;
		default:
			count3 = 0;
			break;
	}
}

int main(void) {
	/* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;
    	

    TimerSet(1);
    TimerOn();
    PORTB = 0x00;
    /* Insert your solution below */
    while (1) {
        while (!TimerFlag);
        TimerFlag = 0;
	ThreeLEDsSM();
	BlinkingLEDSM();
	adjustFreq();
	toggleSound();
	CombineLEDsSM();   	 

	}
    return 1;    

}
