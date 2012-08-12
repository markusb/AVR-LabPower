/*
 * Lab Power
 *
 * Util
 *
 * Backlight
 * - PC0 - OC0A
 */

void util_init();
void util_ledonoff(unsigned char);
void util_TC0_init();
uint8_t util_read_calib_byte(uint8_t);
