/*
 * Lab Power
 *
 * EEPROM
 *
*/

#include <avr/eeprom.h>

extern uint16_t EEMEM ee_checksum;

extern uint16_t EEMEM ee_saved_v;
extern uint16_t EEMEM ee_saved_i;

extern uint16_t EEMEM ee_lcd_contrast;

extern uint16_t EEMEM ee_mem_v[];
extern uint16_t EEMEM ee_mem_i[];

#define EE_CHECKSUM 12345
