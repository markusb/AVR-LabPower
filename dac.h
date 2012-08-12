/*
 * Lab Power
 *
 * DAC
 *
 * Drive the DAC
 * - PA2 - Vset - Desired output voltage
 * - PA3 - Iset - Desired output current limit
 */
 
void dac_init ();
void dac_set (unsigned char, int);
