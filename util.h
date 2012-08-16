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
uint8_t util_read_calib_byte(uint8_t);
int util_wait_ms(int);
char * util_ifmt(int, uint8_t);
void util_eeprom_autosave();
void util_eeprom_restore();
extern volatile int util_sec;
extern volatile int util_ms;

#define AS_TIMEOUT 5
