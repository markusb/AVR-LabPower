/*
 * Lab Power
 *
 * Switch
 *
 * Read the rotary switches and buttons
 *
 */

#include <stdio.h>
#include <stddef.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "switch.h"
#include "rotary.h"
#include "util.h"


int but_sec;
int but_ms;

volatile int icount = 0;

static uint8_t lastc;

static volatile uint8_t rot_v = R_START;
static volatile uint8_t rot_i = R_START;

#define ROTBUFMAX 32
static volatile char rotbuf[ROTBUFMAX];
volatile uint8_t rotbwptr = 0;
volatile uint8_t rotbrptr = 0;

/*
// Initialize ports for switch reading
*/
void sw_init () {
    // Pushbuttons
    PORTA.DIRCLR = BUT_S2_PIN | BUT_S3_PIN;
    PORTC.DIRCLR = BUT_S1_PIN;
    // Enable pull-ups
    PORTA.PIN2CTRL = PORT_OPC_PULLUP_gc;
    PORTA.PIN3CTRL = PORT_OPC_PULLUP_gc;
    PORTC.PIN1CTRL = PORT_OPC_PULLUP_gc;

    // Rotary switches
    PORTD.DIRCLR = ROT_I_PINS | ROT_V_PINS | BUT_I_PIN | BUT_V_PIN;
    // Enable pullups
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

/*
// Rotary switch interrupt
// Every level change generates an interrupt
// A valid rotary change is stored in the buffer
// TODO: Improve buffer overflow detection, currently fails at the buffer end
*/
ISR(PORTD_INT0_vect) {
    uint8_t c;

    // read the input ports and look it up in the state table
    rot_v = ttable[rot_v & 0x0f][PORTD.IN & ROT_V_PINS];
    rot_i = ttable[rot_i & 0x0f][(PORTD.IN & ROT_I_PINS)>>3];

    // check for valid rotation events
    c=0;
   	if ((rot_v&0x30) == DIR_CW)  { c=ROT_V_CW;  }
   	if ((rot_v&0x30) == DIR_CCW) { c=ROT_V_CCW; }
   	if ((rot_i&0x30) == DIR_CW)  { c=ROT_I_CW;  }
   	if ((rot_i&0x30) == DIR_CCW) { c=ROT_I_CCW; }

    // store valid event in the buffer
   	if (c>0) {
   	    if (rotbwptr+1 != rotbrptr) {   // check for buffer overflow
   	        rotbuf[rotbwptr++] = c;
            if (rotbwptr>=ROTBUFMAX) rotbwptr=0;
   	    }
   	}
   	icount++;
}

/*
// Read the input switches
// Rotary encoders are read in an interrupt routine and buffered
// Pushbutton switches are polled and have a long-press function
// - A button press generates the base code, e.g. BUT_I
// - A button release generates the long or short code (BUT_I_SHORT/BUT_I_LONG)
//   depending on the press duration.
*/
uint8_t sw_read () {
    uint8_t c;
    int bdelta;

    // Check for rotary input in the buffer
    if (rotbrptr != rotbwptr) {
        cli();
        c = rotbuf[rotbrptr++];
        if (rotbrptr>=32) rotbrptr=0;
        sei();
        return c;
    }
    c=0;
   	if      (!(PORTD.IN & BUT_V_PIN))  { c=BUT_V;  }
   	else if (!(PORTD.IN & BUT_I_PIN))  { c=BUT_I;  }
//   	else if (!(PORTC.IN & BUT_S1_PIN)) { c=BUT_S1; }
//   	else if (!(PORTB.IN & BUT_S2_PIN)) { c=BUT_S2; }
//   	else if (!(PORTB.IN & BUT_S3_PIN)) { c=BUT_S3; }
   	else { c=0; }

    // Detect the release of a button and return the long or short code,
    // depending on the press duration
    if ((c==0)&&(lastc>0)) {
        bdelta=(util_sec-but_sec)*1000+(util_ms-but_ms);
        if (bdelta>BUT_LONGPRESS_MS) {
            c=lastc+BUT_LONG;
            printf("sw_read: button %x long press delta=%d\n",c,bdelta);
        } else {
            c=lastc+BUT_SHORT;
            printf("sw_read: button %x short press delta=%d\n",c,bdelta);
        }
        lastc=0;
        return c;
    }

   	if (c==lastc) return 0;
   	lastc = c;
    printf("sw_read: button %x pressed\n",c);

    but_sec = util_sec;
    but_ms = util_ms;

   	return c;
}

