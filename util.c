/*
 * Lab Power
 *
 * Util
 *
 * On-Off LED
 * - PC0 - OC0A
 */

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "lcd.h"

volatile int millisec;
volatile int seconds;

/*
    Set clock to internal 32Mhz clock
*/
void util_clockinit () {
    CCP = CCP_IOREG_gc; //Security Signature to modify clock
    // initialize clock source to be 32MHz internal oscillator (no PLL)
    OSC.CTRL = OSC_RC32MEN_bm; // enable internal 32MHz oscillator
    while(!(OSC.STATUS & OSC_RC32MRDY_bm)); // wait for oscillator ready
    CCP = CCP_IOREG_gc; //Security Signature to modify clock
    CLK.CTRL = CLK_SCLKSEL_RC32M_gc; //select sysclock 32MHz osc

}

uint8_t util_read_calib_byte( uint8_t index ) {
    uint8_t result;

    /* Load the NVM Command register to read the calibration row. */
    NVM_CMD = NVM_CMD_READ_CALIB_ROW_gc;
    result = pgm_read_byte(index);

    /* Clean up NVM Command register. */
    NVM_CMD = NVM_CMD_NO_OPERATION_gc;

    return result;
}

/*
* Interrupt on TCC0 overflow = interrupt every ms
*/
ISR(TCC0_OVF_vect) {
    PORTC.OUTTGL = PIN6_bm;
    millisec++;
    if (millisec>=1000) {
        millisec=0;
        seconds++;
    }
}

ISR(TCC0_CCB_vect) {
    PORTC.OUTTGL = PIN6_bm;
    millisec++;
    if (millisec>=1000) {
        millisec=0;
        seconds++;
    }
}

void util_init () {
    PMIC_CTRL = PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;
    CPU_SREG = CPU_I_bm;    // Enable global interrupts

    // Initialize TCC0 for LED PWM and ms interrupt tick
    PORTC.DIRSET = PIN0_bm | PIN1_bm;	// Set PC0 and PC1 as output
    TCC0_PER = 32000;			        // Period length
    TCC0_CTRLA = TC_CLKSEL_DIV1_gc;	    // Clock source = system clock (32Mhz)
    TCC0_CTRLB = TC_WGMODE_SS_gc | TC0_CCAEN_bm | TC0_CCBEN_bm; // Enable A&B outputs, set single slope mode;
    TCC0_CCA = 10000;                   // Initial PWM: 30%
    TCC0_CCB = 10000;                   // Initiel PWM: 30%
    TCC0_INTCTRLA = TC1_OVFINTLVL0_bm;
    TCC0_INTCTRLB = TC1_CCBINTLVL0_bm;

    PORTC.DIRSET = PIN6_bm;
}

void util_ledonoff (unsigned char s) {
//    PORTC.DIRSET = PIN0_bm;
    TCC0_CCA = s * 128;
//    if (s<100) PORTC.OUTSET=PIN0_bm;
//    else PORTC.OUTCLR=PIN0_bm;
}

