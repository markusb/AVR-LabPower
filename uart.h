/*
 * Lab Power
 *
 * usart
 *
*/

int uart_putc(char c, FILE *stream);
void uart_init (void);
extern FILE mystdout;

// Want 9600 baud. Have a 32 MHz clock. BSCALE = 0
// BSEL = ( 32000000 / (2^0 * 16*9600)) -1 = 207
// Fbaud = 32000000 / (2^0 * 16 * (207+1))  = 9615 bits/sec
//#define UART_BSEL 207

// Want 115200 baud. Have a 32 MHz clock. BSCALE = 0
// BSEL = ( 32000000 / (2^0 * 16*115200)) -1 = 15
// Fbaud = 32000000 / (2^0 * 16 * (15+1))  = 125000 bits/sec
//#define UART_BSEL 15

// Want 57600 baud. Have a 32 MHz clock. BSCALE = 0
// BSEL = ( 32000000 / (2^0 * 16*57600)) -1 = 34
// Fbaud = 32000000 / (2^0 * 16 * (34+1))  = 57100 bits/sec
#define UART_BSEL 34

