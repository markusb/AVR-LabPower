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
#include <avr/eeprom.h>
#include "util.h"
#include "labpower.h"
#include "eeprom.h"
#include "lcd.h"

extern int dac_v;
extern int dac_i;
extern int mem_v[];
extern int mem_i[];

volatile int util_ms;
volatile int util_sec;

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
// Interrupt on TCC0 overflow = interrupt every ms
*/
ISR(TCC0_OVF_vect) {
//    PORTC.OUTTGL = PIN6_bm;
    cli();
    util_ms++;
    if (util_ms>=1000) {
    PORTB.OUTTGL = PIN1_bm;
        util_ms=0;
        util_sec++;
    }
    sei();
}

/*
// Format an integer into a floating-point display
// With fixed decimal point
// ifmt(12345,2) -> "123.45"
// ifmt(12345,3) -> "12.345"
// ifmt(12,2)    -> "0.12"
*/
#define IFMT_BUFLEN 8
char ifmt_buf[IFMT_BUFLEN]; // = "abcdefghijk";
//char * ifmt_buf = "abcdefghijk";
char * util_ifmt(int num, uint8_t dp) {
    uint8_t bp;

    // Initialize and start with a \0 at the end of the string
    bp=IFMT_BUFLEN-1;
    ifmt_buf[bp--]='\0';
    // Add the digits after the decimal point
    while(dp--) {
        ifmt_buf[bp--] = (char)0x30+(num%10);
        num = num/10;
    }
    // Add the decimal point
    ifmt_buf[bp--]='.';
    // Add leading zero, if nothing left before the decimal point
    if(!num)ifmt_buf[bp--]='0';
    // Add the digits in front of the decimal point
    while(num){
        ifmt_buf[bp--] = 0x30+(num%10);
        num = num/10;
    }
    return &ifmt_buf[++bp];
}

int ee_v,ee_i,as_ts;
void util_eeprom_autosave() {
    if ((ee_v!=dac_v)||(ee_i!=dac_i)) {     // v or i has changed
        if (as_ts==0) {                     // start timeout
            as_ts = util_sec;               // set timestamp
        } else {
            if (util_sec>(as_ts+AS_TIMEOUT)) {  // Check for timeout
                ee_v = dac_v;
                ee_i = dac_i;
                eeprom_write_word(&ee_checksum,EE_CHECKSUM);
                eeprom_write_word(&ee_saved_v,dac_v);
                eeprom_write_word(&ee_saved_i,dac_i);
                printf("eeprom_autosave: writing to eeprom\n");
                as_ts=0;
            }
        }
    }
}

void util_eeprom_restore() {
    int i;

    printf("eeprom_restore: restoring from eeprom\n");

    i  = eeprom_read_word(&ee_checksum);
//    i  = eeprom_read_word(&ee_checksum+0x2000);
//    i  = ee_checksum;
    if (i!=EE_CHECKSUM) {
        printf("eeprom_restore: checksum failure c=%d, expected: %d, addr: %d\n",i,EE_CHECKSUM,ee_checksum);
        return;
    }

    dac_v = eeprom_read_word(&ee_saved_v);
    dac_i = eeprom_read_word(&ee_saved_i);

    eeprom_read_block(mem_v,&ee_mem_v,sizeof(int)*MEMORY_CH_MAX);
    eeprom_read_block(mem_i,&ee_mem_i,sizeof(int)*MEMORY_CH_MAX);

//    dac_v = eeprom_read_word(&ee_saved_v+0x2000);
//    dac_i = eeprom_read_word(&ee_saved_i+0x2000);
//    dac_v = ee_saved_v;
//    dac_i = ee_saved_i;
}

// Wait until the next 100ms tick and return the number of ms waited
int util_wait_ms (int ms) {
    int delay,t;
    delay = ms-(util_ms%ms);
    t=delay;
//    printf("util_wait_ms: %d ms=%d delay=%d\n",util_ms,ms,delay);
    while (delay--) _delay_ms(1);
//    while ((util_ms%ms)>0);
    return t;
}

void util_init () {
    PMIC_CTRL = PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm;
    CPU_SREG = CPU_I_bm;    // Enable global interrupts

    // Initialize TCC0 for LED PWM and ms interrupt tick
    PORTC.DIRSET = PIN0_bm | PIN1_bm;	// Set PC0 and PC1 as output
    TCC0_PER = 32000;			        // Period length
    TCC0_CTRLA = TC_CLKSEL_DIV1_gc;	    // Clock source = system clock (32Mhz)
    TCC0_CTRLB = TC_WGMODE_SS_gc | TC0_CCAEN_bm | TC0_CCBEN_bm; // Enable A&B outputs, set single slope mode;
    TCC0_CCA = 10000;                   // Initial PWM: 30%
    TCC0_CCB = 10000;                   // Initiel PWM: 30%
    TCC0_INTCTRLA = TC_OVFINTLVL_MED_gc;
//    TCC0_INTCTRLB = TC1_CCBINTLVL0_bm;

    PORTC.DIRSET = PIN6_bm;
    PORTB.DIRSET = PIN1_bm;
    PORTB.OUTSET = PIN1_bm;

    ee_v = dac_v;
    ee_i = dac_i;
}

void util_ledonoff (unsigned char s) {
//    PORTC.DIRSET = PIN0_bm;
    TCC0_CCA = s * 125;
//    if (s<100) PORTC.OUTSET=PIN0_bm;
//    else PORTC.OUTCLR=PIN0_bm;
}
