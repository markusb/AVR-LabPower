/*
 * Lab Power
 *st7565.h
 * ADC
 *
 * read analog channels
 * - PA1 - Voltmeter
 * - PA4 - Vin - Input Voltage
 * - PA4 - Iucurr - Current on microcurrent device
 * - PA4 - Iout - Output current (main shunt)
 * - PA4 - Vout - Output voltage
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stddef.h>
#include "adc.h"
#include "util.h"

void adc_init () {
    // Calibrate ADC
    ADCA.CALL = util_read_calib_byte( offsetof(NVM_PROD_SIGNATURES_t, ADCACAL0) );
    ADCA.CALH = util_read_calib_byte( offsetof(NVM_PROD_SIGNATURES_t, ADCACAL1) );
    // PORTA is all inputs
    PORTA.DIR = 0;
    PORTA.PIN0CTRL = PORT_ISC0_bm | PORT_ISC1_bm | PORT_ISC2_bm;
    PORTA.PIN1CTRL = PORT_ISC0_bm | PORT_ISC1_bm | PORT_ISC2_bm;
    PORTA.PIN2CTRL = PORT_ISC0_bm | PORT_ISC1_bm | PORT_ISC2_bm;
    PORTA.PIN3CTRL = PORT_ISC0_bm | PORT_ISC1_bm | PORT_ISC2_bm;
    PORTA.PIN4CTRL = PORT_ISC0_bm | PORT_ISC1_bm | PORT_ISC2_bm;
    PORTA.PIN5CTRL = PORT_ISC0_bm | PORT_ISC1_bm | PORT_ISC2_bm;
    PORTA.PIN6CTRL = PORT_ISC0_bm | PORT_ISC1_bm | PORT_ISC2_bm;
    PORTA.PIN7CTRL = PORT_ISC0_bm | PORT_ISC1_bm | PORT_ISC2_bm;

    // Enable the ADCA
    ADCA_CTRLA = ADC_ENABLE_bm;
}


int adc_read (uint8_t channel) {
    int i;
    uint8_t j;

//    PORTA.DIRCLR = (1<<c);
    ADCA_CTRLA = ADC_ENABLE_bm;
    ADCA_CTRLB = ADC_RESOLUTION_12BIT_gc;
//    ADCA_REFCTRL = ADC_REFSEL_INT1V_gc | ADC_BANDGAP_bm | ADC_TEMPREF_bm;
//    ADCA_REFCTRL = ADC_REFSEL_VCC_gc;
    ADCA_REFCTRL = ADC_REFSEL_INT1V_gc;
//    ADCA_PRESCALER = ADC_PRESCALER0_bm | ADC_PRESCALER1_bm; // 32Mhz /32 = 1Mhz
    ADCA_PRESCALER = ADC_PRESCALER_DIV256_gc; // 32Mhz/256 = 125k

    if (channel>10) {
        ADCA.CH0.CTRL = ADC_CH_INPUTMODE_INTERNAL_gc;	 // internal source
        ADCA.CH0.MUXCTRL = ((channel-10)<<3);	 // Temp - Bandgap - Vcc - DAC
//    ADCA.CH0.MUXCTRL = ADC_CH_MUXINT_SCALEDVCC_gc; // Vcc/10
//    ADCA.CH0.MUXCTRL = ADC_CH_MUXINT_BANDGAP_gc;
//    ADCA.CH0.MUXCTRL = ADC_CH_MUXINT_TEMP_gc;
    } else {
        ADCA.CH0.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc;	 // single ended
    }
//    ADCA.CH0.MUXCTRL = c;	 // PORTA:2
//    ADCA.CH0.MUXCTRL = 1;	 // Vin
    ADCA.CH0.MUXCTRL = (channel<<3);	 // Gnd
    _delay_ms(1);
//    ADCA.CH0.CTRL |= ADC_CH_START_bm; // start conversion on channel 0
    i=0;
    for (j=0; j<ADCA_AVGCOUNT; j++) {
        ADCA.CTRLA |= ADC_CH0START_bm;

        while(!ADCA.CH0.INTFLAGS);
        ADCA.CH0.INTFLAGS=ADC_CH_CHIF_bm;
        i += ADCA.CH0RES;
    }
    return (i/ADCA_AVGCOUNT) - ADCA_OFFSET;
}

