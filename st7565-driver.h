
// Display driver for DOGM128

// Display properties (for graphics functions)
#define DISP_WIDTH   128L
#define DISP_HEIGHT  65L
#define DISP_DEPTH   1		// monochrome
#define DISP_RAM_SIZE 1152	// 9 x 128

// Hardware pin definitions
#define A0_PORT  PORTE.OUT
#define A0_DDR   PORTE.DIR
#define A0_BIT   1
#define RES_PORT PORTC.OUT
#define RES_DDR  PORTE.DIR
#define RES_BIT  4

void disp_send_frame();
