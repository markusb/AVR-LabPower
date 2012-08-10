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
#include "lcd.h"

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

/************************************
* Initialize TC0 for LED PWM
*/
void util_TC0_init () {
    PORTC.DIRSET = PIN0_bm | PIN1_bm;	// Set PC0 and PC1 as output
    TCC0_PER = 32768;			// Period length
    TCC0_CTRLA = TC_CLKSEL_DIV1_gc;	// Clock source = system clock (32Mhz)
    TCC0_CTRLB = TC_WGMODE_SS_gc | TC0_CCAEN_bm | TC0_CCBEN_bm; // Enable A,B set single slope mode;
    TCC0_CCA = 16384;
    TCC0_CCB = 16384;
}

void util_init () {
}

void util_ledonoff (unsigned char s) {
//    PORTC.DIRSET = PIN0_bm;
    TCC0_CCA = s * 128;
//    if (s<100) PORTC.OUTSET=PIN0_bm;
//    else PORTC.OUTCLR=PIN0_bm;
}

