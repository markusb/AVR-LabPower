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

// for debugging purposes
extern int icount;
extern uint8_t rotbwptr;
extern uint8_t rotbrptr;

extern int seconds;
extern int millisec;
extern char *build;

//FILE *s;
uint8_t rot_decade=1;
volatile int dac_v;
volatile int dac_i;
volatile int adc_vmeter;
volatile int adc_vin;
volatile int adc_iuc;
volatile int adc_iout;
volatile int adc_vout;
volatile int adc_dac0;
volatile int adc_temp;
volatile int adc_vcc;

int main () {
    int count = 0;
    unsigned char c,d;
    uint8_t t;

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

    while (1) {
        count++;

        disp_clear_dont_refresh();
        lcd_gotoxy(0,1);
        fprintf_P(&LCD,PSTR("MLP3003 V%s %s "),FWVERSION,build);

        if (count>10) {
            util_ledonoff(20);
            count=0;
        } else if (count>9) {
            util_ledonoff(150);
        }
        t=util_wait_ms(100);
//        _delay_ms(50);
//        if (count==0) printf("[%05d:%05d] icount=%d w=%d r=%d \n",seconds,millisec,icount,rotbwptr,rotbrptr);
        if (count==0) printf("[%05d:%05d %d] \n",seconds,millisec,t );

        lcd_gotoxy(0,12);
        fprintf_P(&LCD,PSTR("DAC-Vout: %dV"),dac_v);
        lcd_gotoxy(0,21);
        fprintf_P(&LCD,PSTR("DAC-Iout:  %dA"),dac_i);
//        g_fill_rect(10,10,50,30);

        adc_vmeter = adc_read(1);
        adc_vin = adc_read(4);
//        adc_iuc = adc_read(5);
//        adc_iout = adc_read(6);
        adc_vout = adc_read(7);
        adc_temp = adc_read(10);
        adc_vcc  = adc_read(12);
        adc_dac0 = adc_read(13);

        lcd_gotoxy(0,30);
        fprintf_P(&LCD,PSTR("Vmeter %dV"),adc_vmeter);
        lcd_gotoxy(0,39);
        fprintf_P(&LCD,PSTR("Vin %dV"),adc_vin);
        lcd_gotoxy(0,48);
        fprintf_P(&LCD,PSTR("Iuc %dV"),adc_iuc);
        lcd_gotoxy(0,57);
        fprintf_P(&LCD,PSTR("Iout %dV"),adc_iout);
        lcd_gotoxy(80,12);
        fprintf_P(&LCD,PSTR("%dVout"),adc_vout);
        lcd_gotoxy(80,21);
        fprintf_P(&LCD,PSTR("%dVdac"),adc_dac0);
        lcd_gotoxy(80,30);
        fprintf_P(&LCD,PSTR("%dVcc"),adc_vcc);
        lcd_gotoxy(80,39);
        fprintf_P(&LCD,PSTR("%dTemp"),adc_temp);

//        char * util_ifmt(int, uint8_t
        lcd_gotoxy(80,48);
        fprintf_P(&LCD,PSTR("%s Vm"),util_ifmt(adc_vmeter,1));


        while (c = sw_read()) {
            if (c==ROT_V_CW) {
                dac_v += rot_decade;
                if (dac_v>4000) dac_v = 4000;
                dac_set(0,dac_v);
                printf("main: set_dac(0,%d)\n",dac_v);
            }
            if (c==ROT_V_CCW) {
                dac_v -= rot_decade;
                if (dac_v<0) dac_v = 0;
                dac_set(0,dac_v);
                printf("main: set_dac(0,%d)\n",dac_v);
            }
            if (c==ROT_I_CW) {
                dac_i += rot_decade;
                if (dac_i>4000) dac_i = 4000;
                dac_set(0,dac_i);
                printf("main: set_dac(1,%d)\n",dac_i);
            }
            if (c==ROT_I_CCW) {
                dac_i -= rot_decade;
                if (dac_i<0) dac_i = 0;
                dac_set(0,dac_i);
                printf("main: set_dac(1,%d)\n",dac_i);
            }
            if (c==BUT_V) {
                switch (rot_decade) {
                    case 1: rot_decade = 10; break;
                    case 10: rot_decade = 100; break;
                    case 100: rot_decade = 1; break;
                    default: rot_decade = 1; break;
                }
                printf("main: rot_decade=%d\n",rot_decade);
            }
            if (c==BUT_I) {
                printf("main: but_i\n");
            }
            if (c==BUT_S1) {
                printf("main: but_s1\n");
            }
            if (c==BUT_S2) {
                printf("main: but_s2\n");
            }
            if (c==BUT_S3) {
                printf("main: but_s3\n");
            }
        }
//        lcd_gotoxy(60,48);
//        fprintf_P(&LCD,PSTR("Key %d "),c);

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
