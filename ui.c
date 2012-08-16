/*
 * Lab Power
 *
 * AVR based lab power supply
 *
 * User-Interface and state machine
 *
 */

#include <stdio.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include "ui.h"
#include "lcd.h"
#include "uart.h"
#include "switch.h"
#include "st7565.h"
#include "labpower.h"
#include "util.h"
#include "dac.h"
#include "eeprom.h"

extern char *build;
extern uint8_t lcd_contrast;

extern int dac_v;
extern int dac_i;
extern int adc_vmeter;
extern int adc_vin;
extern int adc_iuc;
extern int adc_iout;
extern int adc_vout;
extern int adc_dac0;
extern int adc_temp;
extern int adc_vcc;
extern int mem_v[];
extern int mem_i[];

uint8_t rot_decade=10;

uint8_t ui_screen=UI_SCN_NORMAL;
uint8_t ui_menuline = 0;
uint8_t ui_memory_ch = 0;

void ui_screen_normal();
void ui_screen_setup();
void ui_screen_error();
void ui_screen_memory();
void ui_screen_message();
void ui_screen_about();

char ui_msgbuf1[40];
int ui_msgtimeout;
int ui_msgsec;
int ui_msgms;

void ui_display() {
//    printf("ui_display:");
    disp_clear_dont_refresh();
    lcd_gotoxy(0,1);
    switch (ui_screen) {
        case UI_SCN_NORMAL: ui_screen_normal(); break;
        case UI_SCN_SETUP:  ui_screen_setup();  break;
        case UI_SCN_MEMORY: ui_screen_memory(); break;
        case UI_SCN_MESSAGE: ui_screen_message(); break;
        case UI_SCN_ABOUT: ui_screen_about(); break;
        default: ui_screen_error(); break;
    }
    disp_send_frame();
}

void ui_screen_normal () {
    uint8_t c;
//    printf("ui_screen_normal: 1");


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


    lcd_gotoxy(90,56);
    fprintf_P(&LCD,PSTR("%03d:%03d"),util_sec,util_ms);

//    _delay_ms(5);

//    lcd_gotoxy(0,56);
//    fprintf_P(&LCD,PSTR("</> E+/-/*e [\] |"));

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
        if (c==BUT_V_SHORT) {
            printf("main: but_v\n");
            ui_screen=UI_SCN_SETUP;
            ui_menuline=0;
        }
        if (c==BUT_I) {
            switch (rot_decade) {
                case 1: rot_decade = 10; break;
                case 10: rot_decade = 100; break;
                case 100: rot_decade = 1; break;
                default: rot_decade = 1; break;
            }
            printf("main: but_i - rot_decade=%d\n",rot_decade);
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
}

void ui_screen_setup () {
    uint8_t c;
//    printf("ui_screen_setup: 1");

    fprintf_P(&LCD,PSTR("MLP3003: Setup Menu"));

    lcd_gotoxy(8,12); fprintf_P(&LCD,PSTR("Memory"));
    lcd_gotoxy(8,21); fprintf_P(&LCD,PSTR("LCD Contrast: %d"),lcd_contrast);
    lcd_gotoxy(8,30); fprintf_P(&LCD,PSTR("About"));
    lcd_gotoxy(8,48); fprintf_P(&LCD,PSTR("Reset"));

    lcd_gotoxy(0,12+(ui_menuline*9));
    lcd_putc('>',&LCD);

    while ((c=sw_read())) {
        if (c==ROT_V_CW) {
            if (ui_menuline<5) ui_menuline++;
        }
        if (c==ROT_V_CCW) {
            if (ui_menuline>0) ui_menuline--;
        }
        if (c==ROT_I_CW) {
            if ((ui_menuline==1)&&(lcd_contrast<30)) lcd_contrast++;
        }
        if (c==ROT_I_CCW) {
            if ((ui_menuline==1)&&(lcd_contrast>10)) lcd_contrast--;
        }
        if (c==BUT_V_SHORT) {
            switch (ui_menuline) {
                case 0: ui_screen=UI_SCN_MEMORY; break;
                case 2: ui_screen=UI_SCN_ABOUT; break;
                default: ui_screen=UI_SCN_NORMAL;
            }
        }
        if (c==BUT_I) {
            ui_screen=UI_SCN_NORMAL;
        }
    }
}

void ui_screen_memory () {
    uint8_t c;
//    printf("ui_screen_setup: 1");

    fprintf_P(&LCD,PSTR("MLP3003: Memory Menu"));

    for (c=0; c<=MEMORY_CH_MAX; c++) {
        lcd_gotoxy(8,12+(c*9));
        fprintf_P(&LCD,PSTR("Mem%d: "),c);
        fprintf_P(&LCD,PSTR("%sV "),util_ifmt(mem_v[c],2));
        fprintf_P(&LCD,PSTR("%sA"),util_ifmt(mem_i[c],3));
    }

    lcd_gotoxy(0,57);
    fprintf_P(&LCD,PSTR("Recall/Set       Cancel"));

    lcd_gotoxy(0,12+(ui_memory_ch*9));
    lcd_putc('>',&LCD);

    while ((c=sw_read())) {
        if (c==ROT_V_CW) {
            if (ui_memory_ch<MEMORY_CH_MAX) ui_memory_ch++;
        }
        if (c==ROT_V_CCW) {
            if (ui_memory_ch>0) ui_memory_ch--;
        }
        if (c==BUT_V_SHORT) { // Recall
            dac_v=mem_v[ui_memory_ch];
            dac_i=mem_i[ui_memory_ch];
            ui_screen=UI_SCN_NORMAL;
        }
        if (c==BUT_V_LONG) { // Save
            mem_v[ui_memory_ch]=dac_v;
            mem_i[ui_memory_ch]=dac_i;
            eeprom_write_word(&ee_mem_v[ui_memory_ch],mem_v[ui_memory_ch]);
            eeprom_write_word(&ee_mem_i[ui_memory_ch],mem_v[ui_memory_ch]);
            ui_screen=UI_SCN_MESSAGE;
            strcpy_PF(ui_msgbuf1,PSTR("Saving to memory"));
            ui_msgtimeout=2000;
        }
        if (c==BUT_I) { // Cancel
            ui_screen=UI_SCN_NORMAL;
        }
    }
}
//    eeprom_read_block(mem_v,&ee_mem_v,sizeof(int)*MEMORY_CH_MAX);
//    eeprom_read_block(mem_i,&ee_mem_i,sizeof(int)*MEMORY_CH_MAX);
//                eeprom_write_word(&ee_saved_v,dac_v);

void ui_screen_error () {
    uint8_t c;
    printf("ui_screen_error:\n");

    fprintf_P(&LCD,PSTR("MLP3003: Error Screen"));
}

void ui_screen_message () {
    uint8_t c;
    //printf("ui_screen_message: d=%d buf1='%s'\n",ui_msgtimeout,ui_msgbuf1);

    fprintf_P(&LCD,PSTR("MLP3003: Message Screen"));

    lcd_gotoxy(5,21);
    fprintf(&LCD,ui_msgbuf1);

    lcd_gotoxy(0,57);
    fprintf_P(&LCD,PSTR("OK"));

    while ((c=sw_read())) {
        if (c==BUT_V) { // Recall
            ui_screen=UI_SCN_NORMAL;
            ui_msgtimeout = 0;
        }
    }
    if (ui_msgtimeout) {
        ui_msgms = util_ms+ui_msgtimeout;
        ui_msgsec += util_sec+(ui_msgms/1000);
        ui_msgms = ui_msgms%1000;
        ui_msgtimeout=0;
    }
    if ((util_sec>=ui_msgsec)&&(util_ms>=ui_msgms)) ui_screen=UI_SCN_NORMAL;
}

void ui_screen_about () {
    uint8_t c;
//    printf("ui_screen_about:\n");

    fprintf_P(&LCD,PSTR("MLP3003: About Screen"));
    util_read_calib_byte( offsetof(NVM_PROD_SIGNATURES_t, ADCACAL0) );
    lcd_gotoxy(0,12); fprintf_P(&LCD,PSTR("Firmware Version: %s"),FWVERSION);
    lcd_gotoxy(0,21); fprintf_P(&LCD,PSTR("Build Number: %s"),build);
    lcd_gotoxy(0,30); fprintf_P(&LCD,PSTR("Ser: %x:%x:%x:%x:%x:%x-%x"),
            util_read_calib_byte( offsetof(NVM_PROD_SIGNATURES_t, LOTNUM0)),
            util_read_calib_byte( offsetof(NVM_PROD_SIGNATURES_t, LOTNUM1)),
            util_read_calib_byte( offsetof(NVM_PROD_SIGNATURES_t, LOTNUM2)),
            util_read_calib_byte( offsetof(NVM_PROD_SIGNATURES_t, LOTNUM3)),
            util_read_calib_byte( offsetof(NVM_PROD_SIGNATURES_t, LOTNUM4)),
            util_read_calib_byte( offsetof(NVM_PROD_SIGNATURES_t, LOTNUM5)),
            util_read_calib_byte( offsetof(NVM_PROD_SIGNATURES_t, WAFNUM))
        );
    lcd_gotoxy(0,39); fprintf_P(&LCD,PSTR("By Markus B{rtschi"));
    lcd_gotoxy(0,48); fprintf_P(&LCD,PSTR("markus@markus.org"));

    while ((c=sw_read())) {
        if (c==BUT_V_SHORT) { // Recall
            ui_screen=UI_SCN_NORMAL;
            ui_msgtimeout = 0;
        }
    }
}

