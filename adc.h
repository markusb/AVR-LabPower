/*
 * Lab Power
 *
 * ADC
 *
 * read analog channels
 * - PA1 - Voltmeter
 * - PA4 - Vin - Input Voltage
 * - PA4 - Iucurr - Current on microcurrent device
 * - PA4 - Iout - Output current (main shunt)
 * - PA4 - Vout - Output voltage
 */

#define ADCA_OFFSET 176
#define ADCA_AVGCOUNT 16

void adc_init ();
int adc_read (uint8_t);
