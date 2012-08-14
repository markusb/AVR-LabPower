/*
 * Lab Power
 *
 * Switch
 *
 * Read the rotary switches and buttons
 *
 */

#include <avr/io.h>
#include <stddef.h>
#include "switch.h"
#include "rotary.h"

static volatile uint8_t rot_v = R_START;
static volatile uint8_t rot_i = R_START;

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
}

uint8_t sw_read () {
//    uint8_t pin_v = ((PORTD.IN & ROT_V_PINS));
//    uint8_t pin_i = ((PORTD.IN & ROT_I_PINS)>>3);

//    rot_v = ttable[rot_v & 0x0f][pin_v];
//    rot_i = ttable[rot_i & 0x0f][pin_i];

//    printf("sw_read: pv=%02X sv=%02X - pi=%02X si=%02X\n",pin_v,rot_v,pin_i,rot_i);

    rot_v = ttable[rot_v & 0x0f][PORTD.IN & ROT_V_PINS];
    rot_i = ttable[rot_i & 0x0f][(PORTD.IN & ROT_I_PINS)>>3];

   	if ((rot_v&0x30) == DIR_CW)  { return ROT_V_CW;  }
   	if ((rot_v&0x30) == DIR_CCW) { return ROT_V_CCW; }
   	if ((rot_i&0x30) == DIR_CW)  { return ROT_I_CW;  }
   	if ((rot_i&0x30) == DIR_CCW) { return ROT_I_CCW; }

   	if (!(PORTD.IN & BUT_V_PIN))  { return BUT_V;  }
   	if (!(PORTD.IN & BUT_I_PIN))  { return BUT_I;  }
//   	if (!(PORTC.IN & BUT_S1_PIN)) { return BUT_S1; }
   	if (!(PORTB.IN & BUT_S2_PIN)) { return BUT_S2; }
   	if (!(PORTB.IN & BUT_S3_PIN)) { return BUT_S3; }
   	return 0;
}

