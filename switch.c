/*
 * Lab Power
 *
 * Switch
 *
 * Read the rotary switches and buttons
 *
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stddef.h>
#include "switch.h"
#include "rotary.h"

extern int millisec;
extern int seconds;

static volatile uint8_t rot_v = R_START;
static volatile uint8_t rot_i = R_START;
static volatile char rotbuf[32];
static volatile uint8_t rotbwptr = 0;
static volatile uint8_t rotbrptr = 0;

void sw_init () {
    // Set ports/pins as input
    PORTB.DIRCLR = BUT_S2_PIN | BUT_S3_PIN;
    PORTC.DIRCLR = BUT_S1_PIN;
    PORTD.DIRCLR = ROT_I_PINS | ROT_V_PINS | BUT_I_PIN | BUT_V_PIN;
    // Enable pull-ups
    PORTB.PIN2CTRL = PORT_OPC_PULLUP_gc;
    PORTB.PIN3CTRL = PORT_OPC_PULLUP_gc;
    PORTC.PIN1CTRL = PORT_OPC_PULLUP_gc;

    PORTD.PIN0CTRL = PORT_OPC_PULLUP_gc;
    PORTD.PIN1CTRL = PORT_OPC_PULLUP_gc;
    PORTD.PIN2CTRL = PORT_OPC_PULLUP_gc;
    PORTD.PIN3CTRL = PORT_OPC_PULLUP_gc;
    PORTD.PIN4CTRL = PORT_OPC_PULLUP_gc;
    PORTD.PIN5CTRL = PORT_OPC_PULLUP_gc;

    // Interrupt 0 for rotary inputs
    PORTD.INTCTRL = PORT_INT0LVL_LO_gc;
    PORTD.INT0MASK = ROT_I_PINS | ROT_V_PINS;
}


ISR(PORTD_INT0_vect) {
    PORTC.OUTTGL = PIN6_bm;
    uint8_t c;

    rot_v = ttable[rot_v & 0x0f][PORTD.IN & ROT_V_PINS];
    rot_i = ttable[rot_i & 0x0f][(PORTD.IN & ROT_I_PINS)>>3];

    c=0;
   	if ((rot_v&0x30) == DIR_CW)  { c=ROT_V_CW;  }
   	if ((rot_v&0x30) == DIR_CCW) { c=ROT_V_CCW; }
   	if ((rot_i&0x30) == DIR_CW)  { c=ROT_I_CW;  }
   	if ((rot_i&0x30) == DIR_CCW) { c=ROT_I_CCW; }

   	if (c>=0) {
   	    rotbuf[rotbwptr++] = c;
        if (rotbwptr>=32) rotbwptr=0;
   	}
}

uint8_t sw_read () {
    uint8_t c;
    if (rotbrptr != rotbwptr) {
        cli();
        c = rotbuf[rotbrptr++];
        if (rotbrptr>=32) rotbrptr=0;
        sei();
        return c;
    }
   	if (!(PORTD.IN & BUT_V_PIN))  { return BUT_V;  }
   	if (!(PORTD.IN & BUT_I_PIN))  { return BUT_I;  }
//   	if (!(PORTC.IN & BUT_S1_PIN)) { return BUT_S1; }
   	if (!(PORTB.IN & BUT_S2_PIN)) { return BUT_S2; }
   	if (!(PORTB.IN & BUT_S3_PIN)) { return BUT_S3; }
   	return 0;
}

