

#define ROT_V_PINS (PIN1_bm | PIN1_bm)  // PD0 PD1
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
#define BUT_V       5
#define BUT_I       6
#define BUT_S1      7
#define BUT_S2      8
#define BUT_S3      9

void sw_init();
uint8_t sw_read();
