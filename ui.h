/*
 * Lab Power
 *
 * AVR based lab power supply
 *
 * User-Interface and state machine
 *
 */

#define UI_SCN_NORMAL 1
#define UI_SCN_SETUP 2
#define UI_SCN_MEMORY 3
#define UI_SCN_MESSAGE 4

#define MEMORY_CH_MAX 4

void ui_display();
