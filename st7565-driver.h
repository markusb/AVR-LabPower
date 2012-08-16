/*
*
* Graphic display driver for LCD with ST7565 controller
*
* Originally by Sebastian, see http://www.mikrocontroller.net/topic/75589
*
* Reorganized and modified by Markus Baertschi
*/

// Display properties (for graphics functions)
#define DISP_WIDTH   128L
#define DISP_HEIGHT  64L    // 65 lines broken !
#define DISP_DEPTH   1		// monochrome
//#define DISP_RAM_SIZE 1152	// 9 x 128
#define DISP_RAM_SIZE 1024	// 9 x 128

// Hardware pin definitions
#define CS_PORT  PORTE.OUT
#define CS_DDR   PORTE.DIR
#define CS_BIT   0
#define A0_PORT  PORTE.OUT
#define A0_DDR   PORTE.DIR
#define A0_BIT   1
#define RES_PORT PORTC.OUT
#define RES_DDR  PORTC.DIR
#define RES_BIT  4

void disp_init();
void disp_send_frame();
void disp_clear();
void disp_clear_dont_refresh();
void disp_set_pixel(uint8_t,uint8_t,uint8_t);
