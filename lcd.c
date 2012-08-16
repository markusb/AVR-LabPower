/*
 * Lab Power
 *
 * LCD
 *
 * Drive the NHD-C12865AR LCD
 * - SPI Mode (hardcoded)
 * - PE0 - CS
 * - PE1 - A0
 * - PC4 - R - Reset
 * - PC5 - SI - SPI Data
 * - PC7 - SCL - SPI Clock
 *
 * Backlight
 * - PC1 - OC0B
 */

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"
#include "st7565.h"
//#include "conf_st7565r.h"
//#include <common/components/display/st7565r/st7565r.h>

#define LCD_CS PIN0_bm
#define LCD_A0 PIN1_bm
#define LCD_R PIN4_bm
#define LCD_SCL PIN7_bm
#define LCD_SI PIN5_bm

uint8_t lcd_contrast=LCD_ST7565R_DEFAULT_CONTRAST;
FILE LCD = FDEV_SETUP_STREAM (lcd_putc, NULL, _FDEV_SETUP_WRITE);

/*
* initialisation:
* RES low for 10 ms
* CS low
* A0 low = control
* A0 high = data
* SPI speed min 50ns/cycle -> max 20Mhz
*/
void lcd_init () {
    // Setup ports
    printf("lcd_init\n");

    // set up ports
    PORTE.DIRSET = LCD_CS | LCD_A0;
    PORTC.DIRSET = LCD_R | LCD_SCL | LCD_SI;
    PORTE.OUTSET = LCD_CS | LCD_A0;
    PORTC.OUTSET = LCD_R;
    _delay_ms(1);

    // Reset the display (1ms)
    PORTC.OUTCLR = LCD_R;
    _delay_ms(1);
    PORTC.OUTSET = LCD_R;

    // Setup SPI
    PORTC.DIRSET = LCD_SCL | LCD_SI;
//    SPIC_CTRL = SPI_ENABLE_bm | SPI_MASTER_bm | SPI_MODE0_bm | SPI_MODE1_bm | SPI_PRESCALER0_bm;
    // Idle=high, sample on rising clock, /2 clock = 16 Mhz
    SPIC_CTRL = SPI_ENABLE_bm | SPI_MASTER_bm | SPI_MODE0_bm | SPI_MODE1_bm | SPI_CLK2X_bm;

    PORTE.OUTCLR = LCD_CS | LCD_A0;

    // send init commands
    // A0 - ADC normal
    // AE - Display off
    // C0 - Output scan direction: normal
    // A2 - Bias 0  1/65 duty cycle, 1/9 bias
    // 2F - Power controller: all on
    // 26 - V0 voltage regulator set
    // 81 - Set contrast
    // 2F - Contrast value 0-63/0-3F

    // AF - display on
    lcd_sendcmd(LCD_ST7565R_CMD_OFF);
    lcd_sendcmd(LCD_ST7565R_CMD_ADCNORMAL);	// Normal top down
    lcd_sendcmd('\xC0');
//    lcd_sendcmd('\xDF');
    lcd_sendcmd(LCD_ST7565R_CMD_ADCREVERSE);
    lcd_sendcmd('\x2F');
    lcd_sendcmd('\x26');
//    lcd_sendcmd('\x00');
//    lcd_sendcmd('\x10');
    lcd_setcontrast(lcd_contrast);
    lcd_sendcmd(LCD_ST7565R_CMD_ON);

    // A0 - ADC select - Normal
    // A1 - ADC select - Reverse
    // A2 - Bias 0
    // A3 - Bias 1
    // A4 - All pixels normal
    // A5 - All pixels on
    // A6 - Reverse Display Off
    // A7 - Reverse Display On
    // AA - Sleep mode on (+x)
    // AB - Sleep mode off (+x)
    // AE - Display off
    // AF - Display on
    // 4-7x - Display start line set
    // Bx - Page address set
    // 0x - Column address set lower nibble
    // 1x - Column address set upper nibble
    // E0 - Read-modify-write begin
    // EE - End
    // E2 - Reset
    // Cx - Common output mode select C0/DF
    // 2x - Power controller set
    // 2x - V0 voltage regulator set
    // 81 - Electronic volume mode set (+x)
    // F8 - Booster ratio set (+x)
    // E3 - NOP
    // FF - Test (do not use)
}

/***********************
* Set backlight strength
*/
void lcd_setbacklight (uint8_t s) {
    TCC0_CCB = s * 125;
//    PORTC.DIRSET = PIN1_bm;
//    if (s) PORTC.OUTSET=PIN1_bm;
//    else PORTC.OUTCLR=PIN1_bm;
}

void lcd_setcontrast (uint8_t c) {
    if ((c<4)||(c>60)) return;
    lcd_sendcmd(LCD_ST7565R_CMD_CONTRAST);
    lcd_sendcmd(c);
}

void lcd_clearscreen () {
    unsigned char p,x;
    lcd_sendcmd('\x40');  // Set start line
    for (p=0; p<9; p++) {
//        lcd_sendcmd('\xB0' + p);  // Set page
        lcd_sendcmd('\xB0'+p);  // Set page
        lcd_sendcmd('\x00');  // Column address
        lcd_sendcmd('\x10');  // Column address
        for (x=0; x<128; x++) lcd_senddata(0);
//        for (x=0; x<132; x++) lcd_senddata((1<<x)|(128>>x));
    }
}

/*
*  Simple stdio adaptation to write to the LCD
*/
static uint8_t lcd_x;  // Drawing position
static uint8_t lcd_y;

// Write a char to the current position with a 1 px space afterwards
int lcd_putc (char c, FILE *stream) {
    uint8_t i;
    if((c<' ')||(c>127)) return 0;  // We only have charcters between space and 127

    lcd_x += g_draw_char_clearBG(lcd_x, lcd_y, c);

    // Clear a single pixer row after the character
    for (i=0; i<8; i++) disp_set_pixel(lcd_x, lcd_y+i, 0);
    lcd_x++;

    return 1;
}

// Set the current position
void lcd_gotoxy(uint8_t x, uint8_t y) {
    lcd_x = x;
    lcd_y = y;
}


void lcd_putcxy (unsigned char c,unsigned char x,unsigned char y) {
    unsigned char i;
    lcd_sendcmd('\xB0'+x);  // Set page
//    lcd_sendcmd('\x00'+(x&15));  // Column address lower nibble
//    lcd_sendcmd('\x10'+(x/16));  // Column address upper nibble
    lcd_sendcmd('\x10');  // Column address upper nibble
    lcd_senddata('\xFF');
    for (i=0; i<6; i++) lcd_senddata('\x81');
    lcd_senddata('\xFF');
}


void lcd_sendspi (unsigned char c) {
    unsigned char dummy;

    SPIC.DATA = c;
    while ( !( SPIC.STATUS & SPI_IF_bm) );
    dummy = SPIC.DATA;
}

void lcd_sendcmd (unsigned char c) {
//    printf("lcd_sendcmd: %02x\n",c);
    // Select command mode
    PORTE.OUTCLR = LCD_A0;
    lcd_sendspi(c);
}

void lcd_senddata (unsigned char c) {
    // Select data mode
//    printf("lcd_senddata: %02x\n",c);
    PORTE.OUTSET = LCD_A0;
    lcd_sendspi(c);
}


