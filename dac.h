/*
 * Lab Power
 *
 * DAC
 *
 * Drive the DAC
 * - PA2 - Vset - Desired output voltage
 * - PA3 - Iset - Desired output current limit
 */

#define DAC_CH0OFFSETCAL    0x00
#define DAC_CH0GAINCAL      0x00
#define DAC_CH1OFFSETCAL    0x00
#define DAC_CH1GAINCAL      0x00

#define DAC_V 0
#define DAC_I 1

void dac_init ();
void dac_set (unsigned char, int);
