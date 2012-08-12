/*
 * Lab Power
 *
 * usart
 *
*/

int uart_putc(char c, FILE *stream);
void uart_init (void);
extern FILE mystdout;

//#define UART_BSEL 207
// BSEL = ( 32000000 / (2^0 * 16*9600)) -1 = 207
// Fbaud = 32000000 / (2^0 * 16 * (207+1))  = 9615 bits/sec

#define UART_BSEL 15
// BSEL = ( 32000000 / (2^0 * 16*115200)) -1 = 15
// Fbaud = 32000000 / (2^0 * 16 * (207+1))  = 125000 bits/sec

// Want 9600 baud. Have a 32 MHz clock. BSCALE = 0
// BSEL = ( 2000000 / (2^0 * 16*9600)) -1 = 12
// Fbaud = 2000000 / (2^0 * 16 * (12+1))  = 9615 bits/sec

