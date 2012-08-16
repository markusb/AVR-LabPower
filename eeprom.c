/*
 * Lab Power
 *
 * EEPROM
 *
*/

#include <avr/eeprom.h>
#include "labpower.h"

uint16_t EEMEM ee_checksum = 12345;

uint16_t EEMEM ee_saved_v = 200;
uint16_t EEMEM ee_saved_i = 500;

uint16_t EEMEM ee_lcd_contrast = 18;

uint16_t EEMEM ee_mem_v[MEMORY_CH_MAX];
uint16_t EEMEM ee_mem_i[MEMORY_CH_MAX];

