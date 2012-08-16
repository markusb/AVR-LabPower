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
#include "ui.h"

// for debugging purposes
//extern int icount;
//extern uint8_t rotbwptr;
//extern uint8_t rotbrptr;

extern int seconds;
extern int millisec;
extern char *build;

extern uint8_t lcd_contrast;

volatile int dac_v=100;
volatile int dac_i=1000;
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
    int t;

    int tempref = util_read_calib_byte( offsetof(NVM_PROD_SIGNATURES_t, TEMPSENSE0)) +
                  util_read_calib_byte( offsetof(NVM_PROD_SIGNATURES_t, TEMPSENSE1)) * 256;

    uart_init();
    stdout = &mystdout;
    util_clockinit();
    printf("MLP3003 V%s, build %s initializing\n",FWVERSION,build);

    // Set up LEDs with PWM
    util_init();
    util_ledonoff(20);
    lcd_setbacklight(100);

    sw_init();
    adc_init();
    dac_init();
    dac_set(DAC_V,dac_v);
    dac_set(DAC_I,dac_i);

    lcd_init();

    while (1) {
        count++;
        if (count>10) {
            util_ledonoff(20);
            count=0;
        } else if (count>9) {
            util_ledonoff(150);
        }
        t=util_wait_ms(100);
        if (count==0) printf("[%04d:%03d %d] \n",seconds,millisec,t );

        adc_vmeter = adc_read(1);
        adc_vin = adc_read(4);
        adc_iuc = adc_read(5);
        adc_iout = adc_read(6);
        adc_vout = adc_read(7);
        adc_temp = adc_read(10);
        adc_vcc  = adc_read(12);
        adc_dac0 = adc_read(13);
        disp_clear_dont_refresh();

/*
        lcd_gotoxy(0,1);
        fprintf_P(&LCD,PSTR("MLP3003 V%s %s "),FWVERSION,build);

        lcd_gotoxy(0,12);
        fprintf_P(&LCD,PSTR("DAC:"));
        lcd_gotoxy(52,12);
        fprintf_P(&LCD,PSTR("%sV"),util_ifmt(dac_v,2));
        lcd_gotoxy(90,12);
        fprintf_P(&LCD,PSTR("%sA"),util_ifmt(dac_i,3));


        lcd_gotoxy(0,21);
        fprintf_P(&LCD,PSTR("ADC:"));
        lcd_gotoxy(52,21);
        fprintf_P(&LCD,PSTR("%sV"),util_ifmt(adc_vout,2));
        lcd_gotoxy(90,21);
        fprintf_P(&LCD,PSTR("%sA"),util_ifmt(adc_iout,3));

        lcd_gotoxy(0,30);
        fprintf_P(&LCD,PSTR("Meter:"));
        lcd_gotoxy(52,30);
        fprintf_P(&LCD,PSTR("%sV"),util_ifmt(adc_vmeter-(adc_vmeter/11)-(adc_vmeter/45),2));

        lcd_gotoxy(0,39);
        fprintf_P(&LCD,PSTR("Vin:"));
        lcd_gotoxy(52,39);
        fprintf_P(&LCD,PSTR("%sV"),util_ifmt(adc_vin,2));
        lcd_gotoxy(90,39);
        fprintf_P(&LCD,PSTR("%sAuc"),util_ifmt(adc_iuc,2));

        lcd_gotoxy(0,48);
        fprintf_P(&LCD,PSTR("Vcc:"));
        lcd_gotoxy(52,48);

        fprintf_P(&LCD,PSTR("%sV"),util_ifmt(adc_vcc>>2,2));
        lcd_gotoxy(90,48);
        fprintf_P(&LCD,PSTR("%sT"),util_ifmt(adc_temp,2));

        while ((c=sw_read())) {
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
//                lcd_contrast++;
                dac_i += rot_decade;
                if (dac_i>4000) dac_i = 4000;
                dac_set(1,dac_i);
                printf("main: set_dac(1,%d)\n",dac_i);
            }
            if (c==ROT_I_CCW) {
//                lcd_contrast--;
                dac_i -= rot_decade;
                if (dac_i<0) dac_i = 0;
                dac_set(1,dac_i);
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

        disp_send_frame();
*/
        lcd_setcontrast(lcd_contrast);

        lcd_setbacklight(250-((adc_vin-500)/10));
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
