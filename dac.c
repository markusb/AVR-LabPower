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

void dac_init () {
//    DACA.CTRLA = DAC_IDOEN_bm | DAC_CH1EN_bm | DAC_ENABLE_bm;
    DACB.CTRLB = 0;
    DACB.CTRLC = 0;
    DACB.CTRLA = DAC_IDOEN_bm | DAC_ENABLE_bm;
//    DACB.CTRLB = DAC_CHSEL1_bm;
//    DAC_CH0GAINCAL =
//    DAC_CH0OFFSETCAL =
//    DAC_CH1GAINCAL =
//    DAC_CH1OFFSETCAL =
    DACB.CH0DATA = 500;
}

void dac_set (uint8_t c, int v) {
    if (c==0) DACB.CH0DATA = v;
    if (c==1) DACB.CH1DATA = v;
}

int dac_test () {
    return 0;
}

