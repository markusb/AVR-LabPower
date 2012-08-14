/*
 * Lab Power
 *
 * AVR based lab power supply
 *
 */

#include <stdio.h>
#include <stddef.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "util.h"
#include "lcd.h"
#include "adc.h"
#include "dac.h"
#include "uart.h"
#include "switch.h"
#include "labpower.h"
#include "st7565.h"


extern int seconds;
extern int millisec;
extern char *build;

//FILE *s;
char buf[20];
volatile int dac_v;
volatile int dac_i;
volatile int adc_vmeter;
volatile int adc_vin;
volatile int adc_iuc;
volatile int adc_iout;
volatile int adc_vout;
volatile int adc_dacv;

int main () {
    int count = 0;
    int mainc = 0;
    unsigned char c,d;

    int tempref = util_read_calib_byte( offsetof(NVM_PROD_SIGNATURES_t, TEMPSENSE0)) +
                  util_read_calib_byte( offsetof(NVM_PROD_SIGNATURES_t, TEMPSENSE1)) * 256;

    uart_init();
    stdout = &mystdout;
    util_clockinit();
    printf("MLP3003 V%s, build %s initializing\n",FWVERSION,build);

    // Set up LEDs with PWM
    util_init();
    util_ledonoff(20);
    lcd_setbacklight(200);

    sw_init();
    adc_init();
    dac_init();

    lcd_init();
//    g_draw_rect(5,5,118,55);
//    disp_init();
    lcd_clearscreen();

//    g_draw_string(2,2,"MLP-3003 Lab Power !",0);
    lcd_gotoxy(0,1);
    fprintf_P(&LCD,PSTR("MLP3003 V%s %s "),FWVERSION,build);

    while (1) {
        count++;
        PORTC.OUTTGL = PIN6_bm;

        if (count>50) {
            util_ledonoff(20);
            count=0;
        } else if (count>45) {
            util_ledonoff(150);
        }
        if (count==0) printf("[%05d:%05d %05d] main loop\n",seconds,millisec,mainc++);

        lcd_gotoxy(0,12);
        fprintf_P(&LCD,PSTR("DAC-Vout: %dV"),dac_v);
        lcd_gotoxy(0,21);
        fprintf_P(&LCD,PSTR("DAC-Iout:  %dA"),dac_i);
//        g_fill_rect(10,10,50,30);

        adc_vmeter = adc_read(1);
        adc_vin = adc_read(4);
        adc_iuc = adc_read(5);
        adc_iout = adc_read(6);
        adc_vout = adc_read(7);
        adc_dacv = adc_read(13);

        lcd_gotoxy(0,30);
        fprintf_P(&LCD,PSTR("Vmeter %dV"),adc_vmeter);
        lcd_gotoxy(0,39);
        fprintf_P(&LCD,PSTR("Vin %dV"),adc_vin);
        lcd_gotoxy(0,48);
        fprintf_P(&LCD,PSTR("Iuc %dV"),adc_iuc);
        lcd_gotoxy(0,57);
        fprintf_P(&LCD,PSTR("Iout %dV"),adc_iout);
        lcd_gotoxy(80,12);
        fprintf_P(&LCD,PSTR("%dV"),adc_dacv);

        c = sw_read();
        switch(c) {
            case ROT_V_CW: dac_v++; dac_set(0,dac_v); break;
            case ROT_V_CCW: dac_v--; dac_set(1,dac_i);break;
            case ROT_I_CW: dac_i++; break;
            case ROT_I_CCW: dac_i--; break;
            case BUT_V: dac_v=100; dac_set(0,dac_v); break;
            case BUT_I: dac_i=100; dac_set(1,dac_i); break;
            case BUT_S2: d-=10; break;
            case BUT_S3: d+=10; break;
        }
        lcd_gotoxy(60,48);
        fprintf_P(&LCD,PSTR("Key %d "),c);

        disp_send_frame();
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
