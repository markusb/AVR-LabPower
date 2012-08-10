/*
 * Lab Power
 *
 * AVR based lab power supply
 *
 */

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include "util.h"
#include "lcd.h"
#include "uart.h"
#include "version.h"
#include "Dogm/utility/NHD12865AR.h"

FILE *s;

int main () {
    int count = 0;
    unsigned char c,d;

    uart_init();
    stdout = &mystdout;
    util_clockinit();
    printf("MLP-3003 V%s initializing\n",FWVERSION);

    // Set up LEDs with PWM
    util_TC0_init(); 
    util_ledonoff(20);
    lcd_setbacklight(200);

    lcd_init();
//    dog_spi_init();
//    dog_Init(1);
//    dog_SetContrast(22);

    
//    uart_putc('A',s);
/*
    lcd_sendcmd('\xB0');  // Page 1
    lcd_sendcmd('\x40');  // Column 2
//    lcd_sendcmd('\xCF');  // Output direction upside down

    // Clear screen
    for (d=0; d<1; d++) {
        lcd_sendcmd('\xB0' + d);  // Set page
        lcd_sendcmd('\x40');  // Set column
        for (c=0; c<132; c++) lcd_senddata(255);
    }
*/
    lcd_clearscreen();

    lcd_putcxy('a',1,2);
    lcd_putcxy('a',3,2);
    lcd_putcxy('a',2,2);
    lcd_putcxy('a',2,1);
    lcd_putcxy('a',2,3);

    while (1) {
        util_ledonoff(20);
	printf("[%05d] main loop\n",count++);
        _delay_ms(500);
//        dog_SetInvertPixelMode(1);
//        lcd_sendcmd('\xA6');
//        lcd_sendcmd(LCD_ST7565R_CMD_PIXELNORM);
        util_ledonoff(150);
        _delay_ms(50);
//        dog_SetInvertPixelMode(0);
//        lcd_sendcmd('\xA7');
//        lcd_sendcmd(LCD_ST7565R_CMD_PIXELON);
//	printf("main loop\n");
//    uart_putc('B',s);
//        vary_lcd();
    }
}

#define BRI_STEP 10
#define BRI_MAX 250
#define BRI_MIN 100
unsigned static char bright=BRI_MAX,dir=0;
void vary_lcd () {

    if (dir==1) {
        bright += BRI_STEP;
        if (bright>BRI_MAX) { bright = BRI_MAX; dir=0; }
    } else {
        bright -= BRI_STEP;
        if (bright>BRI_MIN) { bright = BRI_MIN; dir=1; }
    }
    lcd_setbacklight(bright);
}
