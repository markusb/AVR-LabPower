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


extern char *build;

extern uint8_t lcd_contrast;

volatile int dac_v=330;
volatile int dac_i=500;
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
//    unsigned char c,d;
    int t = 0;

//    int tempref = util_read_calib_byte( offsetof(NVM_PROD_SIGNATURES_t, TEMPSENSE0)) +
//                  util_read_calib_byte( offsetof(NVM_PROD_SIGNATURES_t, TEMPSENSE1)) * 256;

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
        if (count==0) printf("[%04d:%03d %d] \n",util_sec,util_ms,t);

        adc_vmeter = adc_read(1);
        adc_vin = adc_read(4);
        adc_iuc = adc_read(5);
        adc_iout = adc_read(6);
        adc_vout = adc_read(7);
        adc_temp = adc_read(10);
        adc_vcc  = adc_read(12);
        adc_dac0 = adc_read(13);

//        _delay_ms(5);
        ui_display();

        lcd_setcontrast(lcd_contrast);

        // Set the LCD backlight brightness according to the input voltage
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
