

#define ROT_V_PINS (PIN0_bm | PIN1_bm)  // PD0 PD1
#define ROT_I_PINS (PIN3_bm | PIN4_bm)  // PD3 PD4
#define BUT_V_PIN  PIN2_bm              // PD2
#define BUT_I_PIN  PIN5_bm              // PD5
#define BUT_S1_PIN PIN1_bm              // PC1
#define BUT_S2_PIN PIN2_bm              // PB2
#define BUT_S3_PIN PIN3_bm              // PB3

#define ROT_V_CW    1
#define ROT_V_CCW   2
#define ROT_I_CW    3
#define ROT_I_CCW   4
#define BUT_V       0x11
#define BUT_I       0x12
#define BUT_S1      0x13
#define BUT_S2      0x14
#define BUT_S3      0x15
#define BUT_SHORT    0x10
#define BUT_V_SHORT  0x21
#define BUT_I_SHORT  0x22
#define BUT_S1_SHORT 0x23
#define BUT_S2_SHORT 0x24
#define BUT_S3_SHORT 0x25
#define BUT_LONG    0x20
#define BUT_V_LONG  0x31
#define BUT_I_LONG  0x32
#define BUT_S1_LONG 0x33
#define BUT_S2_LONG 0x34
#define BUT_S3_LONG 0x35

void sw_init();
uint8_t sw_read();
