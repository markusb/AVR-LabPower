/*
 * Lab Power
 *
 * AVR based lab power supply
 *
 */

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stddef.h>
#include "util.h"
#include "lcd.h"
#include "adc.h"
#include "dac.h"
#include "uart.h"
#include "labpower.h"
#include "st7565.h"

//#include "Dogm/utility/NHD12865AR.h"

FILE *s;
char buf[20];

int main () {
    int count = 0;
    int adc;
    unsigned char c,d;

    int tempref = util_read_calib_byte( offsetof(NVM_PROD_SIGNATURES_t, TEMPSENSE0)) +
                  util_read_calib_byte( offsetof(NVM_PROD_SIGNATURES_t, TEMPSENSE1)) * 256;

    uart_init();
    stdout = &mystdout;
    util_clockinit();
    printf("MLP-3003 V%s initializing\n",FWVERSION);

    // Set up LEDs with PWM
    util_TC0_init();
    util_ledonoff(20);
    lcd_setbacklight(200);

    adc_init();

    lcd_init();
    disp_clear();
    g_draw_rect(0,0,128,65);
    g_draw_rect(5,5,118,55);
//    disp_init();
//    lcd_clearscreen();

    g_draw_string(4,0,"MLP-3003 Lab Power !",0);
    while (1) {
        util_ledonoff(20);
	printf("[%05d] main loop\n",count++);
        _delay_ms(500);
//        dog_SetInvertPixelMode(1);
//        lcd_sendcmd('\xA6');
//        lcd_sendcmd(LCD_ST7565R_CMD_PIXELNORM);
//        g_fill_rect(10,10,50,30);
        g_draw_string(4,10,"ADC-Vmeter:",0);
        adc = adc_read(1);
        sprintf(buf,"%d  ",adc);
        g_draw_string(80,10,buf,0);

        g_draw_string(4,19,"ADC-Vin:",0);
        adc = adc_read(4);
        sprintf(buf,"%d  ",adc);
        g_draw_string(80,19,buf,0);

        g_draw_string(4,28,"Temp:",0);
        adc = adc_read(10);
        sprintf(buf,"%d  ",adc);
        g_draw_string(80,28,buf,0);

        g_draw_string(4,37,"Vcc:",0);
        adc = adc_read(2);
        sprintf(buf,"%d  ",adc/4);
        g_draw_string(80,37,buf,0);

        g_draw_string(4,46,"DAC:",0);
        adc = adc_read(13);
        sprintf(buf,"%d  ",adc);
        g_draw_string(80,46,buf,0);

        g_draw_string(4,55,"Power/Tempref:",0);
        sprintf(buf,"%d  ",tempref);
        g_draw_string(80,55,buf,0);

        disp_send_frame();
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
