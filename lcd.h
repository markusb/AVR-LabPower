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
 * - PC7 - SCL - SPI Clock
 * - PC5 - SI - SPI Data
 *
 * Backlight
 * - PC1 - OC0A
 */

#define LCD_ST7565R_CMD_CONTRAST '\x81'
#define LCD_ST7565R_CMD_ADCNORMAL '\xA0'
#define LCD_ST7565R_CMD_ADCREVERSE '\xA1'
#define LCD_ST7565R_CMD_BIAS_1_9 '\xA2'
#define LCD_ST7565R_CMD_BIAS_1_7 '\xA3'
#define LCD_ST7565R_CMD_PIXELNORMAL '\xA4'
#define LCD_ST7565R_CMD_PIXELALLON '\xA5'
#define LCD_ST7565R_CMD_REVERSEOFF '\xA6'
#define LCD_ST7565R_CMD_REVERSEON '\xA7'
#define LCD_ST7565R_CMD_OFF '\xAE'
#define LCD_ST7565R_CMD_ON '\xAF'
#define LCD_ST7565R_CMD_READ_M_WRITE '\xE0'
#define LCD_ST7565R_CMD_RMW_END '\xEE'
#define LCD_ST7565R_CMD_RESET '\xE2'
#define LCD_ST7565R_CMD_NOP '\xE3'

void lcd_init ();
void lcd_setbacklight (uint8_t);
void util_clockinit ();
void lcd_clearscreen ();
void lcd_sendcmd (unsigned char);
void lcd_senddata (unsigned char);
void lcd_putcxy (unsigned char, unsigned char, unsigned char);

#define LCD_SETCONTRAST(c) lcd_sendcmd('\x81');lcd_sendcmd(c);
extern FILE LCD;
int lcd_putc (char, FILE *);
void lcd_gotoxy(uint8_t, uint8_t);
