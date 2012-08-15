/*
 * Lab Power
 *
 * DAC
 *
 * Drive the DAC
 * - PA2 - Vset - Desired output voltage
 * - PA3 - Iset - Desired output current limit
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stddef.h>
#include "dac.h"
#include "util.h"

void dac_init () {
//    DACA.CTRLA = DAC_IDOEN_bm | DAC_CH1EN_bm | DAC_ENABLE_bm;
    DACB.CTRLB = DAC_CHSEL_DUAL_gc;
    DACB.CTRLC = ADC_REFSEL_INT1V_gc;
//    DACB.CTRLC = ADC_REFSEL_AREFA_gc;
//    DACB.CTRLA = DAC_IDOEN_bm | DAC_ENABLE_bm;
    DACB.CTRLA = DAC_CH0EN_bm | DAC_CH1EN_bm;
    DACB.CH0GAINCAL = util_read_calib_byte( offsetof(NVM_PROD_SIGNATURES_t, DACB0GAINCAL));
    DACB.CH1GAINCAL = util_read_calib_byte( offsetof(NVM_PROD_SIGNATURES_t, DACB1GAINCAL));
    DACB.CH0OFFSETCAL = util_read_calib_byte( offsetof(NVM_PROD_SIGNATURES_t, DACB0OFFCAL));
    DACB.CH1OFFSETCAL = util_read_calib_byte( offsetof(NVM_PROD_SIGNATURES_t, DACB1OFFCAL));
//    DACB.CH0GAINCAL = DAC_CH0GAINCAL;
//    DACB.CH0OFFSETCAL = DAC_CH0OFFSETCAL;
//    DACB.CH1GAINCAL = DAC_CH1GAINCAL;
//    DACB.CH1OFFSETCAL = DAC_CH1OFFSETCAL;
    DACB.CTRLA = DAC_CH0EN_bm | DAC_CH1EN_bm | DAC_ENABLE_bm;
//    DACB.CTRLB = DAC_CHSEL1_bm;
}

void dac_set (uint8_t c, int v) {
    if (c==DAC_V) DACB.CH0DATA = v;
    if (c==DAC_I) DACB.CH1DATA = v;
}

int dac_test () {
    return 0;
}

